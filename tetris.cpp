#include <sstream>
#include "tetris.hpp"

#include <algorithm>
#include <vector>
/*
 * CLASSE PIECE
 */

piece::piece(): m_side(0), m_color(0), m_grid(nullptr) {
}

piece::piece(uint32_t s, uint8_t c): m_side(0), m_color(0), m_grid(nullptr) {
    if (s == 0 || (s & (s - 1)) != 0) {
        throw tetris_exception("Side deve essere una potenza di 2 > 0");
    }
    if (c == 0) {
        throw tetris_exception("Colore deve essere tra 1 e 255");
    }

    // 2. Allocazione memoria
    m_side = s;
    m_color = c;

    // Alloca array di puntatori
    m_grid = new bool*[m_side];

    // Inizializza tutti i puntatori a nullptr
    for (uint32_t i = 0; i < m_side; ++i) {
        m_grid[i] = nullptr;
    }

    try {
        // Alloca ogni riga
        for (uint32_t i = 0; i < m_side; ++i) {
            m_grid[i] = new bool[m_side]();  // Inizializza a false
        }
    } catch (...) {
        // Se qualcosa va storto, libera tutto
        for (uint32_t i = 0; i < m_side; ++i) {
            if (m_grid[i]) {
                delete[] m_grid[i];
            }
        }
        delete[] m_grid;

        // Resetta i membri
        m_side = 0;
        m_color = 0;
        m_grid = nullptr;

        throw;  // Rilancia l'eccezione originale
    }
}


piece::~piece() {
    if (m_grid) {
        for (uint32_t i = 0; i < m_side; ++i) {
            if (m_grid[i]) {
                delete[] m_grid[i];
            }
        }
        delete[] m_grid;
    }
}

piece::piece(piece &&rhs): m_side(rhs.m_side), m_color(rhs.m_color), m_grid(rhs.m_grid){
    rhs.m_side=0;
    rhs.m_color=0;
    rhs.m_grid= nullptr;
}

piece::piece(const piece &rhs): m_side(rhs.m_side), m_color(rhs.m_color), m_grid(nullptr){
    if (rhs.m_grid) {
        m_grid = new bool*[m_side];
        for (uint32_t i = 0; i < m_side; ++i) {
            m_grid[i] = new bool[m_side];
            for (uint32_t j = 0; j < m_side; ++j) {
                m_grid[i][j] = rhs.m_grid[i][j];
            }
        }
    }
}


bool piece::empty(uint32_t i, uint32_t j, uint32_t s) const {
    if(s==0) return true;
    if (i + s > m_side || j + s > m_side) {
        throw tetris_exception("Indice fuori range");
    }
    for(uint32_t r=i; r<i+s; r++){
        for(uint32_t c=j; c<j+s; c++){
            if(m_grid[r][c]) return false;
        }
    }
    return true;
}

bool piece::full(uint32_t i, uint32_t j, uint32_t s) const {
    if(s==0) return true;
    if (i + s > m_side || j + s > m_side) {
        throw tetris_exception("Indice fuori range");
    }
    for(uint32_t r=i; r<i+s; r++){
        for(uint32_t c=j; c<j+s; c++){
            if(!m_grid[r][c]) return false;
        }
    }
    return true;
}

bool piece::empty() const {
    if(m_side==0) return true;
    for(uint32_t r=0; r<m_side;r++){
        for(uint32_t c=0; c<m_side; c++){
            if(m_grid[r][c]) return false;
        }
    }
    return true;
}

bool piece::full() const {
    if(m_side==0) return false;
    for(uint32_t r=0; r<m_side;r++){
        for(uint32_t c=0; c<m_side; c++){
            if(!m_grid[r][c]) return false;
        }
    }
    return true;
}


void piece::rotate() {
    bool** rotated_grid = nullptr;

    try {
        rotated_grid = new bool*[m_side];
        for (uint32_t i = 0; i < m_side; ++i) {
            rotated_grid[i] = new bool[m_side];
        }

        for (uint32_t i = 0; i < m_side; ++i) {
            for (uint32_t j = 0; j < m_side; ++j) {
                rotated_grid[j][m_side - 1 - i] = m_grid[i][j];
            }
        }
    }
    catch (...) {
        if (rotated_grid) {
            for (uint32_t i = 0; i < m_side; ++i)
                delete[] rotated_grid[i];
            delete[] rotated_grid;
        }
        throw;
    }

    for (uint32_t i = 0; i < m_side; ++i)
        delete[] m_grid[i];
    delete[] m_grid;

    m_grid = rotated_grid;
}

void piece::cut_row(uint32_t i) {
    if (i >= m_side) {
        throw tetris_exception("Indice fuori range in cut_row()>=side");
    }

    for(uint32_t r = i; r > 0; r--) {
        for(uint32_t c = 0; c < m_side; c++) {
            m_grid[r][c] = m_grid[r - 1][c];
        }
    }

    for(uint32_t j = 0; j < m_side; j++) {
        m_grid[0][j] = false;
    }
}

//operatori di assegnamento

piece& piece::operator=(const piece &rhs) {
    if (this != &rhs) {
        if (this->m_grid) {
            for (uint32_t i = 0; i < this->m_side; ++i) {
                delete[] this->m_grid[i];
            }
            delete[] this->m_grid;
            this->m_grid = nullptr;
        }
        this->m_side = rhs.m_side;
        this->m_color = rhs.m_color;
        if (rhs.m_grid) {
            this->m_grid = new bool*[this->m_side];
            for (uint32_t i = 0; i < this->m_side; ++i) {
                this->m_grid[i] = new bool[this->m_side];
            }

            for (uint32_t i = 0; i < this->m_side; ++i) {
                for (uint32_t j = 0; j < this->m_side; ++j) {
                    this->m_grid[i][j] = rhs.m_grid[i][j];
                }
            }
        } else {
            this->m_grid = nullptr;
        }
    }
    return *this;
}

piece& piece::operator=(piece &&rhs) {
    if(this!=&rhs){
        if (this->m_grid) {
            for (uint32_t i = 0; i < this->m_side; ++i) {
                delete[] this->m_grid[i];
            }
            delete[] this->m_grid;
            this->m_grid=nullptr;
        }
        m_side=rhs.m_side;
        m_color=rhs.m_color;
        m_grid=rhs.m_grid;
        rhs.m_side=0;
        rhs.m_color=0;
        rhs.m_grid= nullptr;
    }
    return *this;
}

uint32_t piece::side() const {
    return m_side;
}

int piece::color() const {
    return m_color;
}

bool& piece::operator()(uint32_t i, uint32_t j) {
    if (i >= m_side || j >= m_side) {
        throw tetris_exception("Coordinate fuori dal range");
    }
    return m_grid[i][j];
}

bool piece::operator()(uint32_t i, uint32_t j) const {
    if (i >= m_side || j >= m_side) {
        throw tetris_exception("Coordinate fuori dal range");
    }
    return m_grid[i][j];
}


// stampa del pezzo

void piece::print_ascii_art(std::ostream &os) const {

    for (uint32_t i = 0; i < m_side; i++) {
        for (uint32_t j = 0; j < m_side; j++) {
            if (m_grid[i][j]) {
                os << "\033[48;5;" << int(m_color) << "m" << "  " << "\033[m";
            } else {
                os << ' ' << ' ';
            }
        }
        os << std::endl;
    }
}

//Operatori == e !=
bool piece::operator==(const piece &rhs) const {
    if(m_color!=rhs.m_color) return false;
    if(m_side!=rhs.m_side) return false;
    else{
        for(uint32_t i=0; i<m_side;i++){
            for(uint32_t j=0; j<m_side;j++){
                if(m_grid[i][j]!=rhs.m_grid[i][j])
                    return false;
            }
        }
        return true;
    }
}
bool piece::operator!=(const piece &rhs) const {
    return !(*this==rhs);
}

//Operazioni di I/O

/*
 * Analizzo le () e le []
 *
 * Idea: parto con l'estrarre il primo carattere (
 * se il secondo carattere è ) allora setto tutta la matrice a true
 * altrimenti se il secondo carattere è ( so che dopo la griglia deve avere per forza
 * un impostazione mista ovvero TL TR BL BR raggruppato dalla ( e ) e ogni cosa simile deve averne 4 rispettivamente per TL TR BL e BR
 */
void C(std::istream &is, piece& p, uint32_t start, uint32_t end, uint32_t len) {
    if (len == 0) throw std::runtime_error("Struttura invalida");
    char c;
    do {
        if (!is.get(c)) throw tetris_exception("Fine stream inaspettata");
    } while (c == ' ' || c == '\t' || c == '\n');

    if (c == '[') {
        char close;
        do {
            if (!is.get(close)) throw tetris_exception("']' atteso");
        } while (close == ' ' || close == '\t' || close == '\n');

        if (close != ']') throw tetris_exception("']' atteso");

        for (uint32_t i = 0; i < len; ++i) {
            for (uint32_t j = 0; j < len; ++j) {
                p(start + i, end + j) = false;
            }
        }
    }
    else if (c == '(') {
        is >> std::ws;
        char next=is.peek();
        if (next == std::char_traits<char>::eof()) {
            throw tetris_exception("Fine stream inaspettata");
        }
        if (next == ')') {
            is.get();
            for (uint32_t i = 0; i < len; ++i) {
                for (uint32_t j = 0; j < len; ++j) {
                    p(start + i, end + j) = true;
                }
            }
        }
        else {
            if (len == 1) throw std::runtime_error("Struttura invalida");
            uint32_t half = len / 2;
            C(is, p, start, end, half);
            C(is, p, start, end + half, half);
            C(is, p, start + half, end, half);
            C(is, p, start + half, end + half, half);

            char close;
            do {
                if (!is.get(close)) throw tetris_exception("')' atteso");
            } while (close == ' ' || close == '\t' || close == '\n');

            if (close != ')') throw tetris_exception("')' atteso");
        }
    }
    else {
        throw tetris_exception("Carattere inatteso");
    }
}

void D(std::ostream& os, piece const& p, uint32_t start, uint32_t end, uint32_t len){
    if (len == 0) return;

    if (len == 1) {
        if (p(start, end))
            os << "()";
        else
            os << "[]";
        return;
    }
    bool is_empty = p.empty(start, end, len);
    bool is_full = p.full(start, end, len);

    if (is_empty) {
        os << "[]";
    } else if (is_full) {
        os << "()";
    } else {
        os << '(';
        uint32_t half_len = len / 2;
        D(os, p, start, end, half_len);
        D(os, p, start, end + half_len, half_len);
        D(os, p, start + half_len, end, half_len);
        D(os, p, start + half_len, end + half_len, half_len);
        os << ')';
    }
}
std::istream& operator>>(std::istream& is, piece& p) {
    uint32_t side;
    int color;

    if (!(is >> side)) {
        throw tetris_exception("Errore lettura dimensione side");
    }

    if (side == 0 || (side & (side - 1)) != 0) {
        throw tetris_exception("Dimensione side non è una potenza di 2");
    }

    if (!(is >> color)) {
        throw tetris_exception("Errore lettura colore");
    }

    if (color < 1 || color > 255) {
        throw tetris_exception("Colore non valido (deve essere tra 1 e 255)");
    }

    is >> std::ws;

    if (is.peek() == EOF) {
        throw tetris_exception("Struttura pezzo mancante dopo side e colore");
    }

    // Rimuovi il try-catch interno, lascia propagare le eccezioni
    piece temp_p(side, static_cast<uint8_t>(color));
    C(is, temp_p, 0, 0, temp_p.side());
    p = std::move(temp_p);
    return is;
}

std::ostream& operator<<(std::ostream& os, piece const& p){
    os << p.side() << ' ' << p.color() << ' ';
    D(os,p,0,0,p.side());
    return os;
}

/*
 * CLASSE TETRIS
 */

tetris::tetris(): m_score(0), m_width(0),m_height(0), m_field(nullptr){

}

tetris::tetris(uint32_t w, uint32_t h, uint32_t s): m_score(s),m_width(w),m_height(h),m_field(nullptr) {
    if (w == 0) {
        throw tetris_exception("Larghezza deve essere > 0");
    }
    if (h == 0) {
        throw tetris_exception("Altezza deve essere > 0");
    }
}

tetris::tetris(const tetris &rhs): m_score(rhs.m_score), m_width(rhs.m_width), m_height(rhs.m_height), m_field(nullptr) {
    node * pc=rhs.m_field;
    node * tail= nullptr;
    while(pc){
        node* app=new node{pc->tp, nullptr};
        if(!m_field) m_field=app;
        else tail->next=app;
        tail=app;
        pc=pc->next;
    }
}
tetris::tetris(tetris &&rhs): m_score(rhs.m_score), m_width(rhs.m_width), m_height(rhs.m_height), m_field(rhs.m_field) {
    rhs.m_score = 0;
    rhs.m_field = nullptr;
}

tetris::~tetris() {
    while(m_field){
        node * tmp=m_field;
        m_field=m_field->next;
        delete tmp;
    }
}

uint32_t tetris::score() const {
    return m_score;
}

uint32_t tetris::height() const {
    return m_height;
}

uint32_t tetris::width() const {
    return m_width;
}

tetris &tetris::operator=(tetris &&rhs) {
    if(this!= &rhs){
        while(m_field){
            node* temp=m_field;
            m_field=m_field->next;
            delete temp;
        }
        m_score = rhs.m_score;
        m_width = rhs.m_width;
        m_height = rhs.m_height;
        m_field = rhs.m_field;
        rhs.m_score = 0;
        rhs.m_field = nullptr;
    }
    return *this;
}

tetris& tetris::operator=(const tetris& rhs) {
    if (this != &rhs) {
        node* current = m_field;
        while (current) {
            node* next = current->next;
            delete current;
            current = next;
        }
        m_score = rhs.m_score;
        m_width = rhs.m_width;
        m_height = rhs.m_height;
        m_field = nullptr;

        if (rhs.m_field) {
            node* rhs_current = rhs.m_field;
            node** current_ptr = &m_field;

            while (rhs_current) {
                *current_ptr = new node{rhs_current->tp, nullptr};
                current_ptr = &((*current_ptr)->next);
                rhs_current = rhs_current->next;
            }
        }
    }
    return *this;
}

bool tetris::operator!=(const tetris &rhs) const {
    return !(*this==rhs);
}

bool tetris::operator==(const tetris& rhs) const {
        if (m_width != rhs.m_width || m_height != rhs.m_height || m_score != rhs.m_score) {
            return false;
        }

        const_iterator it1 = begin();
        const_iterator it2 = rhs.begin();

        while (it1 != end() && it2 != rhs.end()) {
            if (it1->p != it2->p || it1->x != it2->x || it1->y != it2->y) {
                return false;
            }
            ++it1;
            ++it2;
        }

        return (it1 == end() && it2 == rhs.end());
}



void tetris::add(const piece& p, int x, int y) {
    if (p.empty()) return;
    if (!containment(p, x, y)) {
        throw tetris_exception{"Il pezzo non può essere inserito"};
    }
    tetris_piece tp{p, x, y};
    node* new_node = new node{tp, nullptr};
    new_node->next = m_field;
    m_field = new_node;
}

void tetris::insert(const piece& p, int x) {
    int found_y = -1;

    for (int y = 0; y < static_cast<int>(m_height); ++y) {
        if (containment(p, x, y)) {
            found_y = y;
        } else if (found_y != -1) {
            break;
        }
    }

    if (found_y == -1) {
        for (int y = static_cast<int>(m_height) - 1; y >= 0; --y) {
            if (containment(p, x, y)) {
                found_y = y;
                break;
            }
        }
    }
    if (found_y == -1) {
        throw tetris_exception("GAME OVER");
    }

    add(p, x, found_y);

    bool changed;
    do {
        changed = false;

        for (int row = static_cast<int>(m_height) - 1; row >= 0; --row) {
            bool row_full = true;

            for (uint32_t col = 0; col < m_width && row_full; ++col) {
                bool cell_occupied = false;

                for (iterator it = begin(); it != end() && !cell_occupied; ++it) {
                    const tetris_piece& tp = *it;

                    if (row >= tp.y && row < tp.y + static_cast<int>(tp.p.side())) {
                        int piece_row = (tp.y + static_cast<int>(tp.p.side()) - 1) - row;
                        int piece_col = col - tp.x;

                        if (piece_col >= 0 && piece_col < static_cast<int>(tp.p.side()) &&
                            piece_row >= 0 && piece_row < static_cast<int>(tp.p.side()) &&
                            tp.p(piece_row, piece_col)) {
                            cell_occupied = true;
                        }
                    }
                }

                if (!cell_occupied) {
                    row_full = false;
                }
            }

            if (row_full) {
                for (iterator it = begin(); it != end(); ++it) {
                    tetris_piece& tp = *it;

                    if (tp.y <= row && row < tp.y + static_cast<int>(tp.p.side())) {
                        uint32_t piece_row = (tp.y + static_cast<int>(tp.p.side()) - 1) - row;
                        tp.p.cut_row(piece_row);
                    }
                }

                m_score += m_width;
                changed = true;

                for (iterator it = begin(); it != end(); ++it) {
                    if (it->y > row) {
                        it->y--;
                    }
                }

                row = static_cast<int>(m_height);
                continue;
            }
        }

        node** current = &m_field;
        while (*current) {
            if ((*current)->tp.p.empty()) {
                node* to_delete = *current;
                *current = to_delete->next;
                delete to_delete;
                changed = true;
            } else {
                current = &((*current)->next);
            }
        }

        bool pieces_fell;
        do {
            pieces_fell = false;

            for (iterator it = begin(); it != end(); ++it) {
                tetris_piece& tp = *it;
                int current_y = tp.y;

                if (current_y > 0) {
                    int new_y = current_y - 1;

                    if (new_y >= 0 && containment(tp.p, tp.x, new_y)) {
                        tp.y = new_y;
                        changed = true;
                        pieces_fell = true;
                        break;
                    }
                }
            }
            if (pieces_fell) {
                changed = true;
                break;
            }

        } while (pieces_fell);

    } while (changed);
}

bool tetris::containment(const piece& p, int x, int y) const {
    if (p.empty()) return true;

    if (y < 0) return false;

    for (uint32_t i = 0; i < p.side(); ++i) {
        for (uint32_t j = 0; j < p.side(); ++j) {
            if (p(i, j)) {
                int field_x = x + static_cast<int>(j);
                int field_y = y + static_cast<int>(p.side() - 1 - i);

                if (field_x < 0 || field_x >= static_cast<int>(m_width) ||
                    field_y < 0 || field_y >= static_cast<int>(m_height)) {
                    return false;
                    }

                for (const_iterator it = begin(); it != end(); ++it) {
                    const tetris_piece& existing = *it;
                    if (existing.p.empty()) continue;

                    if (field_x >= existing.x &&
                        field_x < existing.x + static_cast<int>(existing.p.side()) &&
                        field_y >= existing.y &&
                        field_y < existing.y + static_cast<int>(existing.p.side())) {

                        int piece_x = field_x - existing.x;
                        int piece_y = (existing.y + static_cast<int>(existing.p.side()) - 1) - field_y;

                        if (existing.p(piece_y, piece_x)) {
                            return false;
                        }
                        }
                }
            }
        }
    }

    return true;
}

/*
 * CLASSE ITERATOR
 */

tetris::iterator::iterator(tetris::node *ptr) {
    m_ptr=ptr;
}

bool tetris::iterator::operator==(const tetris::iterator &rhs) const {
    return m_ptr==rhs.m_ptr;
}

bool tetris::iterator::operator!=(const tetris::iterator &rhs) const {
    return m_ptr!=rhs.m_ptr;
}

tetris::iterator::reference tetris::iterator::operator*() {
    return m_ptr->tp;
}

tetris::iterator::pointer tetris::iterator::operator->() {
    return &(m_ptr->tp);
}

tetris::iterator &tetris::iterator::operator++() {
    m_ptr=m_ptr->next;
    return *this;
}

tetris::iterator tetris::iterator::operator++(int) {
    iterator it={m_ptr};
    ++(*this);
    return it;
}

tetris::iterator tetris::begin() {
    return {m_field};

}
tetris::iterator tetris::end() {
    return {nullptr};
}

/*
 * CLASSE CONST ITERATOR
 */

tetris::const_iterator::const_iterator(const tetris::node *ptr) {
    m_ptr=ptr;
}

tetris::const_iterator &tetris::const_iterator::operator++() {
    m_ptr = m_ptr->next;
    return *this;
}

tetris::const_iterator::pointer tetris::const_iterator::operator->() const {
    return &(m_ptr->tp);
}

tetris::const_iterator::reference tetris::const_iterator::operator*() const {
    return m_ptr->tp;
}

tetris::const_iterator tetris::const_iterator::operator++(int) {
    const_iterator it = {m_ptr};
    ++(*this);
    return it;
}

bool tetris::const_iterator::operator==(const tetris::const_iterator &rhs) const {
    return m_ptr==rhs.m_ptr;
}

bool tetris::const_iterator::operator!=(const tetris::const_iterator &rhs) const {
    return m_ptr!=rhs.m_ptr;
}

tetris::const_iterator tetris::begin() const {
    return const_iterator(m_field);
}
tetris::const_iterator tetris::end() const {
    return const_iterator(nullptr);
}


void tetris::print_ascii_art(std::ostream& os) const {
    // Simboli di base
    const char EMPTY = ' ';

    // 1. Stampa l'intestazione e lo score
    os << "| SCORE: " << m_score << std::endl;

    // 2. Itera su ogni riga del campo (dall'alto verso il basso)
    for (int r = m_height - 1; r >= 0; r--) {
        os << "|"; // Bordo sinistro

        for (uint32_t c = 0; c < m_width; c++) {
            bool cell_occupied = false;
            uint8_t cell_color = 0;

            // Cerca tra tutti i pezzi quale occupa questa cella
            for (const_iterator it = begin(); it != end(); ++it) {
                const tetris_piece& tp = *it;

                // Calcola coordinate relative al pezzo
                int piece_r = r - tp.y; // riga relativa al pezzo
                int piece_c = c - tp.x; // colonna relativa al pezzo

                // Controlla se la cella è dentro il pezzo e piena
                if (piece_r >= 0 && piece_r < static_cast<int>(tp.p.side()) &&
                    piece_c >= 0 && piece_c < static_cast<int>(tp.p.side()) &&
                    tp.p(piece_r, piece_c)) {
                    cell_occupied = true;
                    cell_color = tp.p.color();
                    break;
                }
            }

            // Stampa la cella
            if (cell_occupied) {
                os << "\033[48;5;" << int(cell_color) << "m" << "  " << "\033[m";
            } else {
                os << "  "; // Due spazi per mantenere l'allineamento
            }
        }

        os << "|" << std::endl; // Bordo destro
    }

    // 3. Stampa il bordo inferiore
    os << "|";
    for (uint32_t i = 0; i < m_width; i++) {
        os << "--";
    }
    os << "|" << std::endl;
}
std::ostream& operator<<(std::ostream& os, tetris const& t){
    os << t.score() << ' ' << t.width() << ' ' << t.height() <<"\n";
    tetris::const_iterator it=t.begin();
    for(;it!=t.end();it++){
        os << it->p << ' ' << it->x << ' ' << it->y <<"\n";
    }
    return os;
}
std::istream& operator>>(std::istream& is, tetris& t) {
    uint32_t score, width, height;

    if (!(is >> score >> width >> height)) {
        throw tetris_exception("Errore lettura score/width/height");
    }

    if (width == 0 || height == 0) {
        throw tetris_exception("Dimensioni campo non valide");
    }

    tetris temp(width, height, score);

    struct TempNode {
        piece p;
        int x, y;
        TempNode* next;
        TempNode(piece const& piece, int x_, int y_)
            : p(piece), x(x_), y(y_), next(nullptr) {}
        ~TempNode() {
            if (next) delete next;
        }
    };

    TempNode* first = nullptr;
    TempNode* last = nullptr;

    try {
        while (is.peek() != EOF && !is.eof()) {
            piece p;
            if (!(is >> p)) break;

            int x, y;
            if (!(is >> x >> y)) break;

            TempNode* newNode = new TempNode(p, x, y);
            if (!first) first = last = newNode;
            else {
                last->next = newNode;
                last = newNode;
            }
        }
    } catch (...) {
        delete first;
        throw;
    }

    TempNode* current = first;
    while (current) {
        temp.add(current->p, current->x, current->y);
        current = current->next;
    }

    delete first;
    t = std::move(temp);
    return is;
}

void test_piece_constructor() {
    std::cout << "=== TEST COSTRUTTORI PIECE ===" << std::endl;

    // Test costruttore base
    piece p1;
    assert(p1.side() == 0);
    assert(p1.color() == 0);
    std::cout << "Costruttore default: OK" << std::endl;

    // Test costruttore con parametri validi
    piece p2(2, 100);
    assert(p2.side() == 2);
    assert(p2.color() == 100);
    std::cout << "Costruttore con parametri validi: OK" << std::endl;

    // Test costruttore con parametri non validi
    bool exception_thrown = false;
    try {
        piece p3(3, 100); // side non potenza di 2
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Side non potenza di 2 bloccato: OK" << std::endl;

    exception_thrown = false;
    try {
        piece p4(2, 0); // colore 0
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Colore 0 bloccato: OK" << std::endl;
}

void test_piece_empty_full() {
    std::cout << "\n=== TEST EMPTY/FULL PIECE ===" << std::endl;

    piece p(2, 100);
    assert(p.empty());
    assert(!p.full());

    p(0,0) = true;
    assert(!p.empty());
    assert(!p.full());

    // Riempi tutto
    for(uint32_t i = 0; i < p.side(); i++) {
        for(uint32_t j = 0; j < p.side(); j++) {
            p(i,j) = true;
        }
    }
    assert(!p.empty());
    assert(p.full());

    std::cout << "Test empty/full: OK" << std::endl;
}

void test_piece_rotate() {
    std::cout << "\n=== TEST ROTAZIONE PIECE ===" << std::endl;

    piece p(2, 100);
    p(0,0) = true;
    p(0,1) = false;
    p(1,0) = false;
    p(1,1) = true;
    p.print_ascii_art(std::cout);
    std::cout<<std::endl;
    p.rotate();
    p.print_ascii_art(std::cout);
    // Dopo rotazione di 90° in senso orario:
    // [true, false]    diventa    [false, true]
    // [false, true]               [false, true] ??
    // Correzione: la rotazione dovrebbe essere:
    // [true, false]    ->    [false, true]
    // [false, true]          [true, false]

    assert(p(0,0) == false);
    assert(p(0,1) == true);
    assert(p(1,0) == true);
    assert(p(1,1) == false);

    std::cout << "Rotazione: OK" << std::endl;
}

void debug_insert_game_over() {
    std::cout << "=== DEBUG insert GAME OVER ===" << std::endl;

    tetris game(6, 8);
    piece p(2, 100);
    p(0,0) = true; p(0,1) = true;
    p(1,0) = true; p(1,1) = true;

    std::cout << "Tetris field: " << game.width() << "x" << game.height() << std::endl;
    std::cout << "Piece side: " << p.side() << std::endl;

    try {
        std::cout << "Provando insert at x=2..." << std::endl;
        game.insert(p, 2);
        std::cout << "✓ Insert riuscito!" << std::endl;
        std::cout << "Score dopo insert: " << game.score() << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ GAME OVER: " << e.what() << std::endl;

        // Debug dettagliato: perché containment fallisce?
        std::cout << "\nTesting containment in tutte le posizioni y:" << std::endl;
        bool found_valid = false;
        for (int y = 7; y >= -1; y--) {
            bool cont = game.containment(p, 2, y);
            std::cout << "  containment(" << 2 << "," << y << "): " << (cont ? "VALIDO" : "NON VALIDO");
            if (cont) {
                std::cout << "  ← ✓ POSIZIONE VALIDA!";
                found_valid = true;
            }
            std::cout << std::endl;
        }

        if (!found_valid) {
            std::cout << "❌ NESSUNA posizione y valida trovata!" << std::endl;
            std::cout << "Possibili problemi:" << std::endl;
            std::cout << "1. containment() restituisce sempre false" << std::endl;
            std::cout << "2. I bordi non sono gestiti correttamente" << std::endl;
            std::cout << "3. Il campo è considerato sempre occupato" << std::endl;
        }
    }
}

void test_parser_debug() {
    std::cout << "=== DEBUG operator>> tetris ===" << std::endl;

    // Test caso semplice che dovrebbe funzionare
    std::stringstream ss1("0 10 20\n2 100 () 5 6");
    tetris t1;
    try {
        ss1 >> t1;
        std::cout << "✓ Caso semplice: OK" << std::endl;
        std::cout << "  Score: " << t1.score() << ", Size: " << t1.width() << "x" << t1.height() << std::endl;

        int count = 0;
        for (auto it = t1.begin(); it != t1.end(); ++it) count++;
        std::cout << "  Pezzi caricati: " << count << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "✗ Caso semplice FALLITO: " << e.what() << std::endl;
    }

    // Test con più pezzi
    std::stringstream ss2("0 8 6\n2 100 () 1 2\n2 200 () 3 4");
    tetris t2;
    try {
        ss2 >> t2;
        std::cout << "✓ Multipli pezzi: OK" << std::endl;

        int count = 0;
        for (auto it = t2.begin(); it != t2.end(); ++it) {
            std::cout << "  Pezzo " << count << " at (" << it->x << "," << it->y << ")" << std::endl;
            count++;
        }

    } catch (const tetris_exception& e) {
        std::cout << "✗ Multipli pezzi FALLITO: " << e.what() << std::endl;
    }
}

void test_parser_errors() {
    std::cout << "=== TEST PARSER ERRORI ===" << std::endl;

    // Test 1: Colore 0
    try {
        std::stringstream ss1("2 0 ()");
        piece p1;
        ss1 >> p1;
        std::cout << "ERRORE: Colore 0 non bloccato!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "OK: Colore 0 bloccato: " << e.what() << std::endl;
    }

    // Test 2: Formato invalido
    try {
        std::stringstream ss2("2 100 invalid");
        piece p2;
        ss2 >> p2;
        std::cout << "ERRORE: Formato invalido non bloccato!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "OK: Formato invalido bloccato: " << e.what() << std::endl;
    }

    // Test 3: Parentesi non bilanciate
    try {
        std::stringstream ss3("2 100 (()");
        piece p3;
        ss3 >> p3;
        std::cout << "ERRORE: Parentesi non bilanciate non bloccate!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "OK: Parentesi non bilanciate bloccate: " << e.what() << std::endl;
    }
}

void test_cut_row_manual() {
    std::cout << "=== TEST MANUALE CUT_ROW ===" << std::endl;

    // Test 1: Pezzo 2x2, cut riga 0
    std::cout << "Test 1 - 2x2 cut_row(0):" << std::endl;
    piece p1(2, 100);
    p1(0,0) = true;  p1(0,1) = true;
    p1(1,0) = false; p1(1,1) = true;

    std::cout << "Prima:" << std::endl;
    std::cout << p1(0,0) << " " << p1(0,1) << std::endl;
    std::cout << p1(1,0) << " " << p1(1,1) << std::endl;

    p1.cut_row(0);

    std::cout << "Dopo:" << std::endl;
    std::cout << p1(0,0) << " " << p1(0,1) << std::endl;
    std::cout << p1(1,0) << " " << p1(1,1) << std::endl;

    // Test 2: Pezzo 4x4, cut riga 1
    std::cout << "\nTest 2 - 4x4 cut_row(1):" << std::endl;
    piece p2(4, 100);
    // Riempi alcune celle
    p2(0,0)=true; p2(0,1)=true;
    p2(1,0)=true; p2(1,1)=true; p2(1,2)=true;
    p2(2,0)=true; p2(2,3)=true;
    p2(3,1)=true; p2(3,2)=true;

    p2.cut_row(1);
    // Verifica il risultato
}

void test_piece_cut_row() {
    std::cout << "\n=== TEST CUT ROW ===" << std::endl;

    // Test caso valido
    piece p(2, 100);
    p(0,0) = true; p(0,1) = true;
    p(1,0) = false; p(1,1) = true;

    p.cut_row(0);

    // Dopo cut_row(0), la riga 0 diventa la vecchia riga 1
    // e la riga 1 diventa vuota
    assert(p(0,0) == false); // dalla vecchia riga 1
    assert(p(0,1) == true);  // dalla vecchia riga 1
    assert(p(1,0) == false); // nuova riga vuota
    assert(p(1,1) == false); // nuova riga vuota

    // Test caso non valido (dovrebbe lanciare eccezione)
    bool exception_thrown = false;
    try {
        p.cut_row(2); // fuori range
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Cut_row fuori range bloccato: OK" << std::endl;
}

void test_piece_copy_move() {
    std::cout << "\n=== TEST COPY/MOVE PIECE ===" << std::endl;

    piece p1(2, 100);
    p1(0,0) = true; p1(0,1) = true;

    // Test copy constructor
    piece p2(p1);
    assert(p1 == p2);
    assert(p2(0,0) == true);
    assert(p2(0,1) == true);

    // Test move constructor
    piece p3(std::move(p1));
    assert(p3.side() == 2);
    assert(p3.color() == 100);
    assert(p3(0,0) == true);
    assert(p3(0,1) == true);

    // Test copy assignment
    piece p4(1, 50);
    p4 = p2;
    assert(p4 == p2);

    // Test move assignment
    piece p5(1, 50);
    p5 = std::move(p3);
    assert(p5.side() == 2);
    assert(p5(0,0) == true);

    std::cout << "Copy/move: OK" << std::endl;
}

void test_piece_streams() {
    std::cout << "\n=== TEST STREAMS PIECE ===" << std::endl;

    // Test input/output valido
    std::stringstream ss1;
    piece p1(2, 100);
    p1(0,0) = true; p1(0,1) = true;
    p1(1,0) = true; p1(1,1) = true;

    ss1 << p1;

    piece p2;
    ss1 >> p2;

    assert(p1 == p2);
    std::cout << "Stream valido: OK" << std::endl;

    // Test input non valido - side non potenza di 2
    std::stringstream ss2("3 100 ()");
    piece p3;
    bool exception_thrown = false;
    try {
        ss2 >> p3;
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Side non potenza di 2 bloccato: OK" << std::endl;

    // Test input non valido - formato errato
    std::stringstream ss3("2 100 invalid");
    piece p4;
    exception_thrown = false;
    try {
        ss3 >> p4;
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Formato non valido bloccato: OK" << std::endl;

    // Test input non valido - colore fuori range
    std::stringstream ss4("2 300 ()");
    piece p5;
    exception_thrown = false;
    try {
        ss4 >> p5;
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Colore fuori range bloccato: OK" << std::endl;
}

void test_tetris_constructor() {
    std::cout << "\n=== TEST COSTRUTTORI TETRIS ===" << std::endl;

    // Test costruttore base
    tetris t1;
    assert(t1.width() == 0);
    assert(t1.height() == 0);
    assert(t1.score() == 0);
    std::cout << "Costruttore default: OK" << std::endl;

    // Test costruttore con parametri
    tetris t2(10, 20, 5);
    assert(t2.width() == 10);
    assert(t2.height() == 20);
    assert(t2.score() == 5);
    std::cout << "Costruttore con parametri: OK" << std::endl;

    // Test parametri non validi
    bool exception_thrown = false;
    try {
        tetris t3(0, 10, 0);
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Dimensioni 0 bloccate: OK" << std::endl;
}

void test_cut_row_specific() {
    std::cout << "=== TEST CUT_ROW SPECIFICO ===" << std::endl;

    // Test con pezzo 4x4
    piece p(4, 100);
    // Riempi alcune celle
    p(0,0)=true; p(0,1)=true;
    p(1,0)=true; p(1,1)=true; p(1,2)=true;
    p(2,0)=true; p(2,3)=true;
    p(3,1)=true; p(3,2)=true;

    std::cout << "Prima di cut_row(1):" << std::endl;
    p.print_ascii_art(std::cout);

    p.cut_row(1);

    std::cout << "Dopo cut_row(1):" << std::endl;
    p.print_ascii_art(std::cout);
}

void test_tetris_containment() {
    std::cout << "\n=== TEST CONTAINMENT TETRIS ===" << std::endl;

    tetris game(6, 8);

    // Pezzo semplice
    piece p(2, 100);
    p(0,0) = true; p(0,1) = true;
    p(1,0) = true; p(1,1) = true;

    // Test containment valido
    assert(game.containment(p, 2, 0) == true);
    assert(game.containment(p, 2, 6) == true);
    std::cout << "Containment base: OK" << std::endl;

    // Test containment non valido (fuori bordo)
    assert(game.containment(p, 5, 0) == false); // fuori a destra
    assert(game.containment(p, 2, 7) == false); // fuori in basso
    assert(game.containment(p, -1, 0) == false); // fuori a sinistra
    std::cout << "Containment fuori bordo: OK" << std::endl;

    // Test containment con collisione
    game.add(p, 2, 2);

    piece p2(2, 200);
    p2(0,0) = true; p2(0,1) = true;
    p2(1,0) = true; p2(1,1) = true;

    // Dovrebbe collidere con il pezzo esistente
    assert(game.containment(p2, 2, 2) == false);
    assert(game.containment(p2, 4, 2) == true); // posizione libera
    std::cout << "Containment con collisione: OK" << std::endl;
}

void test_tetris_add_insert() {
    std::cout << "\n=== TEST ADD/INSERT TETRIS ===" << std::endl;

    // Test add semplice
    tetris game1(6, 8);
    piece p1(2, 100);
    p1(0,0) = true;
    game1.add(p1, 2, 2);
    std::cout << "Add completato: OK" << std::endl;

    // Test insert semplice
    tetris game2(6, 8);
    piece p2(2, 100);
    p2(0,0) = true; p2(0,1) = true;
    p2(1,0) = true; p2(1,1) = true;
    game2.insert(p2, 2);
    std::cout << "Insert completato: OK" << std::endl;

    // Test add non valido
    bool exception_thrown = false;
    try {
        game1.add(p1, 10, 10);
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Add fuori campo bloccato: OK" << std::endl;

    std::cout << "Add/Insert test completato: OK" << std::endl;
}

void test_tetris_copy_move() {
    std::cout << "\n=== TEST COPY/MOVE TETRIS ===" << std::endl;

    tetris t1(6, 8);

    // Usa parametri validi
    piece p(2, 100);
    p(0,0) = true;

    t1.add(p, 2, 2);

    // Test copy constructor
    tetris t2(t1);
    assert(t1.width() == t2.width());
    assert(t1.height() == t2.height());
    assert(t1.score() == t2.score());
    std::cout << "Copy constructor: OK" << std::endl;

    // Test move constructor
    tetris t3(std::move(t1));
    assert(t3.width() == 6);
    assert(t3.height() == 8);
    std::cout << "Move constructor: OK" << std::endl;

    // Test copy assignment
    tetris t4(2, 2);
    t4 = t2;
    assert(t4.width() == t2.width());
    assert(t4.height() == t2.height());
    std::cout << "Copy assignment: OK" << std::endl;

    // Test move assignment
    tetris t5(2, 2);
    t5 = std::move(t3);
    assert(t5.width() == 6);
    assert(t5.height() == 8);
    std::cout << "Move assignment: OK" << std::endl;

    std::cout << "Copy/move tetris: OK" << std::endl;
}

void test_negative_coordinates() {
    std::cout << "=== TEST COORDINATE NEGATIVE ===" << std::endl;

    tetris game(8, 6);
    piece p(2, 100);
    p(0,0) = true; p(0,1) = true;
    p(1,0) = true; p(1,1) = true;

    // Test coordinate negative
    std::cout << "Test x negativo:" << std::endl;
    for (int x = -3; x < 8; x++) {
        bool ok = game.containment(p, x, 0);
        std::cout << "  containment(" << x << ",0): " << (ok ? "OK" : "NO") << std::endl;
    }

    // Prova add con x negativo
    try {
        game.add(p, -2, 0);
        std::cout << "✓ Add con x=-2 riuscito" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ Add con x=-2 fallito: " << e.what() << std::endl;
    }
}

void test_tetris_iterators() {
    std::cout << "\n=== TEST ITERATORI TETRIS ===" << std::endl;

    tetris game(6, 8);

    piece p1(2, 100);
    p1(0,0) = true;
    game.add(p1, 1, 1);

    piece p2(2, 200);
    p2(0,0) = true;
    game.add(p2, 3, 3);

    int count = 0;
    for (auto it = game.begin(); it != game.end(); ++it) {
        count++;
    }
    assert(count == 2);
    std::cout << "Iteratori: OK (" << count << " pezzi iterati)" << std::endl;

    // Test const iterators
    const tetris& const_game = game;
    count = 0;
    for (auto it = const_game.begin(); it != const_game.end(); ++it) {
        count++;
    }
    assert(count == 2);
    std::cout << "Const iteratori: OK" << std::endl;
}

void test_tetris_streams() {
    std::cout << "\n=== TEST STREAMS TETRIS ===" << std::endl;

    tetris game1(6, 8, 100);

    piece p1(2, 100);
    p1(0,0) = true; p1(0,1) = true;
    game1.add(p1, 1, 2);

    piece p2(4, 200);
    p2(0,0) = true; p2(1,1) = true;
    game1.add(p2, 3, 4);

    // Serializza
    std::stringstream ss;
    ss << game1;

    // Deserializza
    tetris game2;
    ss >> game2;

    assert(game1.width() == game2.width());
    assert(game1.height() == game2.height());
    assert(game1.score() == game2.score());
    std::cout << "Stream tetris: OK" << std::endl;
}

void test_edge_cases() {
    std::cout << "\n=== TEST EDGE CASES ===" << std::endl;

    // Test piece vuota
    piece p1(2, 100); // vuota
    assert(p1.empty());
    std::cout << "Piece vuota: OK" << std::endl;

    // Test piece piena
    piece p2(2, 100);
    for(uint32_t i = 0; i < p2.side(); i++) {
        for(uint32_t j = 0; j < p2.side(); j++) {
            p2(i,j) = true;
        }
    }
    assert(p2.full());
    std::cout << "Piece piena: OK" << std::endl;

    // Test tetris vuoto
    tetris t1(4, 4);
    assert(t1.begin() == t1.end());
    std::cout << "Tetris vuoto: OK" << std::endl;

    std::cout << "Edge cases: OK" << std::endl;
}

void test_specific_failures() {
    std::cout << "\n=== TEST SPECIFICI PER I FAILURE DEI REPORT ===" << std::endl;

    // Test per cut_row out-of-bounds
    std::cout << "--- Test cut_row out-of-bounds ---" << std::endl;
    piece p(2, 100);
    bool exception_thrown = false;
    try {
        p.cut_row(2); // Dovrebbe fallire
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "cut_row out-of-bounds bloccato: OK" << std::endl;

    // Test per invalid piece format
    std::cout << "--- Test invalid piece format ---" << std::endl;
    std::stringstream ss_invalid("2 100 invalid");
    piece p2;
    exception_thrown = false;
    try {
        ss_invalid >> p2;
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    std::cout << "Invalid format bloccato: OK" << std::endl;

    // Test per GAME OVER scenarios
    std::cout << "--- Test GAME OVER scenarios ---" << std::endl;
    tetris game(4, 4);
    piece p3(4, 100);
    // Riempi parzialmente il pezzo
    for(uint32_t i = 0; i < 2; i++) {
        for(uint32_t j = 0; j < 4; j++) {
            p3(i,j) = true;
        }
    }

    try {
        game.insert(p3, 0);
        std::cout << "Insert completato senza GAME OVER: OK" << std::endl;
    } catch (const tetris_exception& e) {
        if (std::string(e.what()) == "GAME OVER") {
            std::cout << "GAME OVER catturato: OK" << std::endl;
        } else {
            std::cout << "Altro errore: " << e.what() << std::endl;
        }
    }
}

void debug_containment_edge_cases() {
    std::cout << "=== DEBUG CONTAINMENT EDGE CASES ===" << std::endl;

    tetris game(4, 4);
    piece p(2, 100);
    p(0,0) = true; p(0,1) = true;
    p(1,0) = true; p(1,1) = true;

    // Aggiungi un pezzo esistente
    game.add(p, 0, 0);

    std::cout << "Con pezzo esistente at (0,0):" << std::endl;
    std::cout << "containment(2,0): " << game.containment(p, 2, 0) << " (dovrebbe essere true)" << std::endl;
    std::cout << "containment(0,2): " << game.containment(p, 0, 2) << " (dovrebbe essere true)" << std::endl;
    std::cout << "containment(0,0): " << game.containment(p, 0, 0) << " (dovrebbe essere false - collisione)" << std::endl;
    std::cout << "containment(1,0): " << game.containment(p, 1, 0) << " (dovrebbe essere false - collisione parziale)" << std::endl;

    // Test con pezzo parzialmente fuori campo
    std::cout << "\nTest bordi:" << std::endl;
    std::cout << "containment(-1,0): " << game.containment(p, -1, 0) << " (dovrebbe essere false)" << std::endl;
    std::cout << "containment(3,0): " << game.containment(p, 3, 0) << " (dovrebbe essere false)" << std::endl;
    std::cout << "containment(0,3): " << game.containment(p, 0, 3) << " (dovrebbe essere false)" << std::endl;
}

void debug_insert_complex_cases() {
    std::cout << "=== DEBUG INSERT CASI COMPLESSI ===" << std::endl;

    // Test 1: Inserimento multiplo
    std::cout << "1. Inserimento multiplo di pezzi:" << std::endl;
    tetris game1(6, 8);
    piece p1(2, 100);
    p1(0,0) = true; p1(0,1) = true;
    p1(1,0) = true; p1(1,1) = true;

    try {
        game1.insert(p1, 0);
        std::cout << "   ✓ Primo insert a x=0: OK" << std::endl;

        game1.insert(p1, 2);
        std::cout << "   ✓ Secondo insert a x=2: OK" << std::endl;

        game1.insert(p1, 4);
        std::cout << "   ✓ Terzo insert a x=4: OK" << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "   ✗ FALLITO: " << e.what() << std::endl;
    }

    // Test 2: Pezzo che completa una riga
    std::cout << "\n2. Test completamento riga:" << std::endl;
    tetris game2(4, 4);
    piece p2(2, 200);
    p2(0,0) = true; p2(0,1) = true;
    p2(1,0) = true; p2(1,1) = true;

    // Inserisci pezzi per quasi completare una riga
    try {
        game2.insert(p2, 0);
        std::cout << "   ✓ Insert a x=0: OK" << std::endl;

        game2.insert(p2, 2);
        std::cout << "   ✓ Insert a x=2: OK" << std::endl;

        std::cout << "   Score dopo inserimenti: " << game2.score() << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "   ✗ FALLITO: " << e.what() << std::endl;
    }

    // Test 3: Pezzo con coordinate negative
    std::cout << "\n3. Test pezzo con y negativo:" << std::endl;
    tetris game3(6, 8);
    piece p3(4, 150);
    p3(0,0) = true; // Solo alcune celle piene
    p3(1,1) = true;

    try {
        game3.insert(p3, 1);
        std::cout << "   ✓ Insert pezzo 4x4: OK" << std::endl;

        // Controlla se il pezzo ha coordinate negative
        for (auto it = game3.begin(); it != game3.end(); ++it) {
            std::cout << "   Pezzo at (" << it->x << "," << it->y << ")" << std::endl;
            if (it->y < 0) {
                std::cout << "   ⚠️  Pezzo con y negativo!" << std::endl;
            }
        }

    } catch (const tetris_exception& e) {
        std::cout << "   ✗ FALLITO: " << e.what() << std::endl;
    }
}

void debug_tetris_io() {
    std::cout << "=== DEBUG TETRIS I/O ===" << std::endl;

    // Crea un tetris con dati noti
    tetris t1(6, 8, 100);
    piece p1(2, 50);
    p1(0,0) = true; p1(0,1) = true;
    p1(1,0) = true; p1(1,1) = true;

    piece p2(4, 75);
    p2(0,0) = true; p2(1,1) = true;

    t1.add(p1, 1, 2);
    t1.add(p2, 3, 4);

    // Test operator<<
    std::stringstream ss;
    ss << t1;
    std::string output = ss.str();
    std::cout << "Output di operator<<:" << std::endl;
    std::cout << "=== INIZIO OUTPUT ===" << std::endl;
    std::cout << output;
    std::cout << "=== FINE OUTPUT ===" << std::endl;

    // Test operator>> con l'output generato
    std::stringstream ss2(output);
    tetris t2;
    try {
        ss2 >> t2;
        std::cout << "✓ operator>> ha letto l'output di operator<<" << std::endl;

        // Verifica uguaglianza
        if (t1.score() == t2.score() &&
            t1.width() == t2.width() &&
            t1.height() == t2.height()) {
            std::cout << "✓ Score e dimensioni matchano" << std::endl;
        } else {
            std::cout << "✗ Score/dimensioni NON matchano" << std::endl;
            std::cout << "t1: score=" << t1.score() << " width=" << t1.width() << " height=" << t1.height() << std::endl;
            std::cout << "t2: score=" << t2.score() << " width=" << t2.width() << " height=" << t2.height() << std::endl;
        }

        // Conta pezzi
        int count1 = 0, count2 = 0;
        for (auto it = t1.begin(); it != t1.end(); ++it) count1++;
        for (auto it = t2.begin(); it != t2.end(); ++it) count2++;

        std::cout << "Pezzi in t1: " << count1 << ", in t2: " << count2 << std::endl;

        if (count1 == count2) {
            std::cout << "✓ Numero pezzi matcha" << std::endl;
        } else {
            std::cout << "✗ Numero pezzi NON matcha" << std::endl;
        }

        // Verifica uguaglianza completa
        if (t1 == t2) {
            std::cout << "🎉 TETRIS I/O FUNZIONA PERFETTAMENTE!" << std::endl;
        } else {
            std::cout << "❌ Tetris non sono uguali dopo I/O" << std::endl;
        }

    } catch (const tetris_exception& e) {
        std::cout << "✗ operator>> FALLITO: " << e.what() << std::endl;
    }
}

void debug_tetris_invalid_cases() {
    std::cout << "\n=== DEBUG TETRIS INVALID CASES ===" << std::endl;

    // Test 1: Input con dimensioni 0
    std::stringstream ss1("100 0 8\n2 50 () 1 2");
    tetris t1;
    try {
        ss1 >> t1;
        std::cout << "✗ ERRORE: Dimensioni 0 non bloccate!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✓ OK: Dimensioni 0 bloccate: " << e.what() << std::endl;
    }

    // Test 2: Input malformato (mancano coordinate)
    std::stringstream ss2("0 6 8\n2 50 () 1");
    tetris t2;
    try {
        ss2 >> t2;
        std::cout << "✗ ERRORE: Input malformato non bloccato!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✓ OK: Input malformato bloccato: " << e.what() << std::endl;
    }

    // Test 3: Piece invalido nel mezzo
    std::stringstream ss3("0 6 8\n2 50 () 1 2\n2 0 invalid 3 4");
    tetris t3;
    try {
        ss3 >> t3;
        std::cout << "✓ Input con piece invalido gestito correttamente" << std::endl;
        int count = 0;
        for (auto it = t3.begin(); it != t3.end(); ++it) count++;
        std::cout << "Pezzi caricati: " << count << " (dovrebbe essere 1)" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ ERRORE: " << e.what() << std::endl;
    }
}

void final_comprehensive_test() {
    std::cout << "=== FINAL COMPREHENSIVE TEST ===" << std::endl;

    // Test 1: I/O round-trip
    debug_tetris_io();

    // Test 2: Casi invalid
    debug_tetris_invalid_cases();

    // Test 3: cut_row (il test che falliva)
    std::cout << "\n=== TEST CUT_ROW ===" << std::endl;
    piece p(4, 100);
    p(0,0)=true; p(0,1)=true; p(0,2)=false; p(0,3)=false;
    p(1,0)=true; p(1,1)=true; p(1,2)=true; p(1,3)=false;
    p(2,0)=true; p(2,1)=false; p(2,2)=false; p(2,3)=true;
    p(3,0)=false; p(3,1)=true; p(3,2)=true; p(3,3)=false;

    std::cout << "Prima di cut_row(1):" << std::endl;
    p.print_ascii_art(std::cout);

    p.cut_row(1);

    std::cout << "Dopo cut_row(1):" << std::endl;
    p.print_ascii_art(std::cout);

    // Test 4: insert con ricerca y corretta
    std::cout << "\n=== TEST INSERT Y CORRETTA ===" << std::endl;
    tetris game(4, 4);
    piece p2(2, 200);
    p2(0,0)=true; p2(1,1)=true;

    // Aggiungi un pezzo che occupa spazio
    game.add(p2, 0, 2);

    try {
        // Prova a inserire un pezzo che dovrebbe andare SOTTO quello esistente
        piece p3(2, 150);
        p3(0,0)=true; p3(1,1)=true;
        game.insert(p3, 0);
        std::cout << "✓ Insert trovato posizione y corretta" << std::endl;

        for (auto it = game.begin(); it != game.end(); ++it) {
            std::cout << "Peice at (" << it->x << "," << it->y << ")" << std::endl;
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ Insert fallito: " << e.what() << std::endl;
    }
}

void debug_color_zero() {
    std::cout << "=== DEBUG COLOR ZERO ===" << std::endl;

    // Test 1: Costruttore con colore 0
    try {
        piece p1(2, 0);
        std::cout << "✗ ERRORE: Costruttore con colore 0 non bloccato!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✓ OK: Costruttore blocca colore 0: " << e.what() << std::endl;
    }

    // Test 2: operator>> con colore 0
    std::stringstream ss("2 0 ()");
    piece p2;
    try {
        ss >> p2;
        std::cout << "✗ ERRORE: operator>> con colore 0 non bloccato!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✓ OK: operator>> blocca colore 0: " << e.what() << std::endl;
    }

    // Test 3: operator>> di tetris con piece colore 0
    std::stringstream ss2("0 6 8\n2 0 () 1 2");
    tetris t;
    try {
        ss2 >> t;
        std::cout << "✗ ERRORE: tetris con piece colore 0 non bloccato!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✓ OK: tetris blocca piece colore 0: " << e.what() << std::endl;
    }
}
void debug_insert_complex_pieces() {
    std::cout << "=== DEBUG INSERT PEZZI COMPLESSI ===" << std::endl;

    // Test 1: Pezzo 4x4 con pattern sparso
    tetris game1(8, 6);
    piece p1(4, 100);
    // Pattern sparso (non tutto pieno)
    p1(0,0)=true; p1(0,2)=true;
    p1(1,1)=true; p1(1,3)=true;
    p1(2,0)=true; p1(2,2)=true;
    p1(3,1)=true; p1(3,3)=true;

    try {
        game1.insert(p1, 2);
        std::cout << "✓ Pezzo 4x4 sparso inserito" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ GAME OVER con pezzo sparso: " << e.what() << std::endl;
    }

    // Test 2: Pezzo che tocca i bordi
    tetris game2(6, 8);
    piece p2(2, 200);
    p2(0,0)=true; p2(0,1)=true;
    p2(1,0)=true; p2(1,1)=true;

    // Prova posizioni estreme
    std::cout << "\nTest posizioni estreme:" << std::endl;
    for (int x = -1; x <= 5; x++) {
        try {
            game2.insert(p2, x);
            std::cout << "  x=" << x << ": ✓ OK" << std::endl;
            // Rimuovi per testare il prossimo
            game2 = tetris(6, 8);
        } catch (const tetris_exception& e) {
            std::cout << "  x=" << x << ": ✗ " << e.what() << std::endl;
        }
    }
}
void debug_insert_with_rotation() {
    std::cout << "\n=== DEBUG INSERT CON ROTAZIONE ===" << std::endl;

    tetris game(8, 6);
    piece p(4, 150);
    // Pattern asimmetrico per testare rotazione
    p(0,0)=true; p(0,1)=true;
    p(1,0)=true;
    p(2,2)=true;
    p(3,3)=true;

    std::cout << "Pezzo originale:" << std::endl;
    p.print_ascii_art(std::cout);

    // Rotazione prima di insert (come nei test play_games)
    p.rotate();

    std::cout << "Dopo rotazione:" << std::endl;
    p.print_ascii_art(std::cout);

    try {
        game.insert(p, 2);
        std::cout << "✓ Insert dopo rotazione riuscito" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ GAME OVER dopo rotazione: " << e.what() << std::endl;
    }
}

void debug_tetris_read_specific() {
    std::cout << "\n=== DEBUG TETRIS READ SPECIFICO ===" << std::endl;

    // Input simile a quelli nei test
    std::stringstream ss1("0 8 6\n2 202 () 1 4");
    tetris t1;

    try {
        ss1 >> t1;
        std::cout << "✓ Lettura base OK" << std::endl;
        std::cout << "  Score: " << t1.score() << ", Size: " << t1.width() << "x" << t1.height() << std::endl;

        int count = 0;
        for (auto it = t1.begin(); it != t1.end(); ++it) {
            std::cout << "  Pezzo " << count << " at (" << it->x << "," << it->y << ")" << std::endl;
            count++;
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ Lettura fallita: " << e.what() << std::endl;
    }

    // Input più complesso (come nella specifica)
    std::stringstream ss2("0 8 6\n2 202 () 1 4\n8 28 ([]([]([]()[]())[]([]()[]()))[]([]([]()[][])[][])) -4 7");
    tetris t2;

    try {
        ss2 >> t2;
        std::cout << "\n✓ Lettura multipla OK" << std::endl;
        std::cout << "  Pezzi caricati: ";
        int count = 0;
        for (auto it = t2.begin(); it != t2.end(); ++it) {
            count++;
        }
        std::cout << count << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "\n✗ Lettura multipla fallita: " << e.what() << std::endl;
    }
}
void test_example_from_spec() {
    std::cout << "=== TEST ESEMPIO SPECIFICA ===" << std::endl;

    // Ricrea l'esempio della specifica
    tetris game(8, 20); // Forse height maggiore di 6

    piece p(8, 28);
    // Qui dovresti creare il pezzo esatto dalla stringa...
    // Per ora testiamo con pezzo semplice

    piece simple_p(2, 202);
    simple_p(0,0)=true; simple_p(1,1)=true;

    std::cout << "Test add con coordinate negative:" << std::endl;
    try {
        game.add(simple_p, -1, 0);
        std::cout << "✓ Add con x=-1 riuscito" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ Add fallito: " << e.what() << std::endl;
    }
}

void test_partial_piece_negative_x() {
    std::cout << "=== TEST PEZZO PARZIALE CON x NEGATIVO ===" << std::endl;

    tetris game(8, 6);

    // Pezzo 4x4 con solo le ultime 2 colonne piene
    piece p(4, 100);
    // Colonne 0,1 vuote; colonne 2,3 piene
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            p(i,j) = (j >= 2); // Solo colonne 2,3 piene
        }
    }

    std::cout << "Pattern pezzo (X=pieno, -=vuoto):" << std::endl;
    p.print_ascii_art(std::cout);

    std::cout << "\nTest containment con x negativo:" << std::endl;
    // x=-2: le colonne piene (j=2,3) vanno a x=0,1 (dentro campo)
    std::cout << "containment(-2,0): " << game.containment(p, -2, 0)
              << " (dovrebbe essere TRUE)" << std::endl;

    // x=-3: le colonne piene vanno a x=-1,0 (parzialmente fuori)
    std::cout << "containment(-3,0): " << game.containment(p, -3, 0)
              << " (dovrebbe essere FALSE)" << std::endl;
}

void debug_insert_negative_x() {
    std::cout << "=== DEBUG INSERT CON x NEGATIVO ===" << std::endl;

    tetris game(8, 6);

    // Pezzo 4x4 con solo destra piena (come nell'esempio)
    piece p(4, 150);
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            p(i,j) = (j >= 2); // Solo colonne 2,3 piene
        }
    }

    std::cout << "Pezzo pattern:" << std::endl;
    p.print_ascii_art(std::cout);

    std::cout << "\nTest insert con vari x:" << std::endl;
    for (int x = -3; x <= 5; x++) {
        tetris game_copy(8, 6); // Nuovo gioco ogni volta

        try {
            game_copy.insert(p, x);
            std::cout << "  x=" << x << ": ✓ OK (y=";
            // Trova y usata
            for (auto it = game_copy.begin(); it != game_copy.end(); ++it) {
                std::cout << it->y;
            }
            std::cout << ")" << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "  x=" << x << ": ✗ " << e.what() << std::endl;
        }
    }
}

void test_critical_cases() {
    std::cout << "=== TEST CASI CRITICI ===" << std::endl;

    // Test 1: Pezzo completamente pieno con x negativo
    std::cout << "1. Pezzo 2x2 pieno con x negativo:" << std::endl;
    tetris game1(8, 6);
    piece p1(2, 100);
    // Riempi tutto
    for (uint32_t i = 0; i < 2; i++) {
        for (uint32_t j = 0; j < 2; j++) {
            p1(i,j) = true;
        }
    }

    std::cout << "  containment(-1,0): " << game1.containment(p1, -1, 0)
         << " (dovrebbe essere FALSE)" << std::endl;

    // Test 2: Lettura esatta dall'esempio specifica
    std::cout << "\n2. Lettura esempio specifica:" << std::endl;
    std::stringstream ss("0 8 20\n2 202 () 1 4\n8 28 ([]([]([]()[]())[]([]()[]()))[]([]([]()[][])[][])) -4 7");
    // NOTA: height=20 invece di 6!

    tetris t;
    try {
        ss >> t;
        std::cout << "✓ Lettura OK con height=20" << std::endl;
        std::cout << "  Pezzi: ";
        int count = 0;
        for (auto it = t.begin(); it != t.end(); ++it) {
            std::cout << "(" << it->x << "," << it->y << ") ";
            count++;
        }
        std::cout << "[" << count << " pezzi]" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ Lettura fallita: " << e.what() << std::endl;
    }

    // Test 3: insert con ricerca y che includa negative
    std::cout << "\n3. insert cerca y negative:" << std::endl;
    tetris game3(8, 6);
    piece p3(4, 150);
    p3(0,0)=true; // Solo una cella piena

    try {
        game3.insert(p3, 0);
        std::cout << "✓ Insert riuscito" << std::endl;
        // Verifica se ha usato y negativo
        for (auto it = game3.begin(); it != game3.end(); ++it) {
            std::cout << "  y usata: " << it->y << std::endl;
            if (it->y < 0) {
                std::cout << "  ⚠️  Usata y negativa!" << std::endl;
            }
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ Insert fallito: " << e.what() << std::endl;
    }
}

void test_official_cases() {
    std::cout << "=== TEST CASI UFFICIALI ===" << std::endl;

    // Prova con varie dimensioni che potrebbero essere usate nei test
    std::vector<std::pair<int, int>> dimensions = {
        {8, 6},   // Forse usato
        {8, 20},  // Come funziona
        {10, 20}, // Standard
        {8, 10}   // Compromesso
    };

    std::string input = "0 8 6\n2 202 () 1 4\n8 28 ([]([]([]()[]())[]([]()[]()))[]([]([]()[][])[][])) -4 7";

    for (auto& dim : dimensions) {
        std::cout << "\nTest con width=" << dim.first << ", height=" << dim.second << ":" << std::endl;

        // Modifica le dimensioni nell'input
        std::stringstream ss;
        ss << "0 " << dim.first << " " << dim.second << "\n";
        ss << "2 202 () 1 4\n";
        ss << "8 28 ([]([]([]()[]())[]([]()[]()))[]([]([]()[][])[][])) -4 7";

        tetris t;
        try {
            ss >> t;
            std::cout << "  ✓ Lettura OK" << std::endl;
            std::cout << "  Pezzi caricati: ";
            int count = 0;
            for (auto it = t.begin(); it != t.end(); ++it) {
                count++;
            }
            std::cout << count << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "  ✗ Fallito: " << e.what() << std::endl;
        }
    }
}

void test_cut_row_thorough() {
    std::cout << "=== TEST CUT_ROW DETTAGLIATO ===" << std::endl;

    // Test caso specifico che potrebbe fallire
    piece p(4, 100);

    // Pattern che potrebbe causare problemi
    // Riga 0: alcune celle piene
    // Riga 1: tagliata via
    // Riga 2,3: altre celle

    p(0,0)=true; p(0,1)=false; p(0,2)=true; p(0,3)=false;
    p(1,0)=false; p(1,1)=true; p(1,2)=false; p(1,3)=true;
    p(2,0)=true; p(2,1)=false; p(2,2)=true; p(2,3)=false;
    p(3,0)=false; p(3,1)=true; p(3,2)=false; p(3,3)=true;

    std::cout << "Prima di cut_row(1):" << std::endl;
    p.print_ascii_art(std::cout);

    // Salva copia per confronto
    piece original = p;

    try {
        p.cut_row(1);
        std::cout << "\nDopo cut_row(1):" << std::endl;
        p.print_ascii_art(std::cout);

        // Verifiche post-cut_row
        std::cout << "\nVerifiche:" << std::endl;
        std::cout << "1. Pezzo vuoto? " << (p.empty() ? "SI" : "NO")
                  << " (dovrebbe essere NO)" << std::endl;
        std::cout << "2. Pezzo pieno? " << (p.full() ? "SI" : "NO")
                  << " (dovrebbe essere NO)" << std::endl;
        std::cout << "3. Side ancora valido? " << p.side()
                  << " (dovrebbe essere 4)" << std::endl;

        // Test operator== dopo cut_row
        std::cout << "4. Pezzo uguale a se stesso? " << (p == p ? "SI" : "NO")
                  << " (dovrebbe essere SI)" << std::endl;

        // Test I/O dopo cut_row
        std::stringstream ss;
        ss << p;
        std::cout << "5. Serializzazione: " << ss.str() << std::endl;

        piece p2;
        ss >> p2;
        std::cout << "6. Deserializzazione OK? " << (p == p2 ? "SI" : "NO")
                  << " (dovrebbe essere SI)" << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "✗ Exception: " << e.what() << std::endl;
    }
}

void test_with_spec_example() {
    std::cout << "=== TEST CON DATI SPECIFICA ===" << std::endl;

    // Dati esatti dalla specifica
    std::string input =
        "0 8 6\n"
        "2 202 () 1 4\n"
        "8 28 ([]([]([]()[]())[]([]()[]()))[]([]([]()[][])[][])) -4 7\n"
        "4 27 (([][]()())([][]()())(()()[][])()) 4 5\n"
        "4 160 (([][]()())([][]()())[][]) 2 7";

    std::stringstream ss(input);
    tetris t;

    try {
        ss >> t;
        std::cout << "✓ Lettura OK" << std::endl;
        std::cout << "Pezzi caricati: ";
        int count = 0;
        for (auto it = t.begin(); it != t.end(); ++it) {
            count++;
        }
        std::cout << count << std::endl;

        // Verifica output
        std::stringstream ss2;
        ss2 << t;
        std::cout << "Output:\n" << ss2.str() << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "✗ Fallito: " << e.what() << std::endl;

        // Debug: perché fallisce?
        if (std::string(e.what()) == "Il pezzo non può essere inserito") {
            std::cout << "Problema con containment per qualche pezzo" << std::endl;
        }
    }
}
void test_height_20_vs_6() {
    std::cout << "=== TEST HEIGHT 20 vs 6 ===" << std::endl;

    // Stessi dati, diverse height
    std::string input_20 =
        "0 8 20\n"
        "2 202 () 1 4\n"
        "8 28 ([]([]([]()[]())[]([]()[]()))[]([]([]()[][])[][])) -4 7\n";

    std::string input_6 =
        "0 8 6\n"
        "2 202 () 1 4\n"
        "8 28 ([]([]([]()[]())[]([]()[]()))[]([]([]()[][])[][])) -4 7\n";

    // Test height=20
    {
        std::stringstream ss(input_20);
        tetris t;
        try {
            ss >> t;
            std::cout << "✓ Height=20: OK" << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "✗ Height=20: " << e.what() << std::endl;
        }
    }

    // Test height=6
    {
        std::stringstream ss(input_6);
        tetris t;
        try {
            ss >> t;
            std::cout << "✓ Height=6: OK" << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "✗ Height=6: " << e.what() << std::endl;
        }
    }

    // Forse y=7 è valido se il pezzo ha celle solo nella parte bassa?
    // Testiamo con un pezzo che ha celle solo nelle righe 0-1
    std::cout << "\nTest pezzo con celle solo in basso:" << std::endl;
    tetris game(8, 6);
    piece p(8, 28);
    // Solo prime 2 righe piene
    for (uint32_t i = 0; i < 2; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            p(i,j) = true;
        }
    }

    // y=7: bottom-left a y=7, ma pezzo alto 8 -> celle a y=7..14
    // Ma height=6 -> y=7..14 sono TUTTE fuori!
    std::cout << "containment con pezzo 8x8 (solo righe 0-1 piene) a y=7: "
              << game.containment(p, -4, 7) << std::endl;
}

void test_insert_no_negative_y() {
    std::cout << "=== TEST INSERT NO NEGATIVE Y ===" << std::endl;

    tetris game(8, 6);
    piece p(4, 100);
    p(0,0)=true; // Solo una cella piena

    try {
        game.insert(p, 2);
        std::cout << "✓ Insert riuscito" << std::endl;

        for (auto it = game.begin(); it != game.end(); ++it) {
            std::cout << "Pezzo a y=" << it->y;
            if (it->y < 0) {
                std::cout << " ⚠️  NEGATIVO!" << std::endl;
            } else {
                std::cout << " ✅ OK (non negativo)" << std::endl;
            }
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ GAME OVER: " << e.what() << std::endl;
    }
}
void test_cut_row_edge_case() {
    // Test con pezzo che diventa vuoto dopo cut_row
    piece p(2, 100);
    p(0,0)=true; p(0,1)=true;  // Solo prima riga piena
    p(1,0)=false; p(1,1)=false;

    p.cut_row(0);  // Taglia l'unica riga piena

    // Ora il pezzo dovrebbe essere vuoto
    if (p.empty()) {
        std::cout << "✓ Pezzo diventato vuoto dopo cut_row - OK" << std::endl;
    } else {
        std::cout << "✗ Pezzo NON vuoto dopo cut_row - PROBLEMA!" << std::endl;
    }
}

void test_io_edge_cases() {
    std::cout << "=== TEST I/O EDGE CASES ===" << std::endl;

    // Test 1: Piece con formato "borderline"
    std::vector<std::string> test_pieces = {
        "2 100 ()",      // Base case
        "2 100 []",      // Vuoto
        "4 150 (()())",  // Formato ricorsivo semplice
        "4 150 ([][])",  // Quadranti misti
    };

    for (const auto& input : test_pieces) {
        std::stringstream ss(input);
        piece p;

        try {
            ss >> p;
            std::stringstream ss2;
            ss2 << p;
            std::string output = ss2.str();

            if (input == output) {
                std::cout << "✓ '" << input << "' → round-trip OK" << std::endl;
            } else {
                std::cout << "⚠ '" << input << "' → output diverso: '"
                          << output << "'" << std::endl;
            }
        } catch (const tetris_exception& e) {
            std::cout << "✗ '" << input << "' → Exception: " << e.what() << std::endl;
        }
    }

    // Test 2: Tetris I/O
    std::cout << "\nTest Tetris I/O:" << std::endl;
    std::string tetris_input = "0 8 6\n2 100 () 1 2";
    std::stringstream ss3(tetris_input);
    tetris t;

    try {
        ss3 >> t;
        std::stringstream ss4;
        ss4 << t;
        std::string tetris_output = ss4.str();

        std::cout << "Input:  '" << tetris_input << "'" << std::endl;
        std::cout << "Output: '" << tetris_output << "'" << std::endl;

        if (tetris_input + "\n" == tetris_output) {
            std::cout << "✓ Tetris round-trip OK" << std::endl;
        } else {
            std::cout << "⚠ Tetris output diverso!" << std::endl;
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ Tetris exception: " << e.what() << std::endl;
    }
}

void test_explicit_formats() {
    std::cout << "=== TEST FORMATI ESPLICITI ===" << std::endl;

    // Formati corretti per side=4 (4 quadranti)
    std::vector<std::string> valid_4x4 = {
        "4 100 (()()()())",      // Tutti pieni
        "4 100 ([][][][])",      // Tutti vuoti
        "4 100 (()[][][]())",    // TL e BR pieni
        "4 100 ((()())()()())",  // Ricorsivo
    };

    for (const auto& input : valid_4x4) {
        std::stringstream ss(input);
        piece p;

        try {
            ss >> p;
            std::cout << "✓ '" << input << "' → OK" << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "✗ '" << input << "' → " << e.what() << std::endl;
        }
    }

    // Forse i test usano side=2?
    std::cout << "\nTest side=2:" << std::endl;
    std::vector<std::string> valid_2x2 = {
        "2 100 ()",    // Pieno
        "2 100 []",    // Vuoto
        "2 100 (())",  // ❌ Questo è invalido! Per side=2, o () o []
    };

    for (const auto& input : valid_2x2) {
        std::stringstream ss(input);
        piece p;

        try {
            ss >> p;
            std::cout << "✓ '" << input << "' → OK" << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "✗ '" << input << "' → " << e.what() << std::endl;
        }
    }
}

void test_abbreviated_formats() {
    std::cout << "=== TEST FORMATI ABBREVIATI ===" << std::endl;

    // Casi sospetti
    std::vector<std::pair<std::string, std::string>> tests = {
        {"4 150 (()())", "Formato abbreviato?"},
        {"4 150 ([][])", "Formato abbreviato?"},
        {"4 150 (()()()())", "Formato esplicito completo"},
        {"4 150 ([][][][])", "Formato esplicito completo"},
    };

    for (const auto& test : tests) {
        std::stringstream ss(test.first);
        piece p;

        try {
            ss >> p;
            std::cout << "✓ '" << test.first << "' → ACCETTATO ("
                      << test.second << ")" << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "✗ '" << test.first << "' → BLOCCATO: "
                      << e.what() << " (" << test.second << ")" << std::endl;
        }
    }

    // Forse side=2 con formato strano?
    std::cout << "\nTest side=2:" << std::endl;
    std::stringstream ss2("2 100 (())");
    piece p2;
    try {
        ss2 >> p2;
        std::cout << "✓ '2 100 (())' → ACCETTATO" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ '2 100 (())' → BLOCCATO: " << e.what() << std::endl;
    }
}

void final_integration_test() {
    std::cout << "=== TEST INTEGRATION FINALE ===" << std::endl;

    // Test 1: Piece I/O completo
    std::cout << "1. Piece I/O:" << std::endl;
    std::string piece_input = "4 150 (()())";
    std::stringstream ss1(piece_input);
    piece p1;

    try {
        ss1 >> p1;
        std::stringstream ss1_out;
        ss1_out << p1;
        std::cout << "  Input:  " << piece_input << std::endl;
        std::cout << "  Output: " << ss1_out.str() << std::endl;
        std::cout << "  ✓ Piece I/O OK" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "  ✗ Piece I/O failed: " << e.what() << std::endl;
    }

    // Test 2: Tetris I/O completo
    std::cout << "\n2. Tetris I/O:" << std::endl;
    std::string tetris_input = "0 8 6\n2 100 () 1 2\n4 150 (()()) 3 4";
    std::stringstream ss2(tetris_input);
    tetris t1;

    try {
        ss2 >> t1;
        std::stringstream ss2_out;
        ss2_out << t1;
        std::cout << "  Input:\n" << tetris_input << std::endl;
        std::cout << "  Output:\n" << ss2_out.str() << std::endl;
        std::cout << "  ✓ Tetris I/O OK" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "  ✗ Tetris I/O failed: " << e.what() << std::endl;
    }

    // Test 3: insert con pezzi complessi
    std::cout << "\n3. Insert complesso:" << std::endl;
    tetris t2(8, 6);
    piece p2(4, 200);
    // Pattern complesso
    std::stringstream ss3("4 200 (()())");
    ss3 >> p2;

    try {
        t2.insert(p2, 2);
        std::cout << "  ✓ Insert pezzo complesso OK" << std::endl;
        std::cout << "  Score: " << t2.score() << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "  ✗ Insert failed: " << e.what() << std::endl;
    }

    // Test 4: cut_row integration
    std::cout << "\n4. cut_row integration:" << std::endl;
    piece p3(4, 250);
    // Solo prima riga piena
    for (uint32_t j = 0; j < 4; j++) p3(0,j) = true;

    p3.cut_row(0);
    std::cout << "  Pezzo dopo cut_row(0): "
              << (p3.empty() ? "vuoto" : "non vuoto") << std::endl;
    std::cout << "  ✓ cut_row integration OK" << std::endl;
}

void debug_parser_expansion() {
    std::cout << "=== DEBUG PARSER EXPANSION ===" << std::endl;

    // Test cosa fa veramente il parser
    std::string input = "4 150 (()())";
    std::stringstream ss(input);
    piece p;

    try {
        ss >> p;
        std::cout << "Parsing di '" << input << "'" << std::endl;

        // Stampa la griglia
        std::cout << "Griglia risultante (4x4):" << std::endl;
        for (uint32_t i = 0; i < 4; i++) {
            for (uint32_t j = 0; j < 4; j++) {
                std::cout << (p(i,j) ? 'X' : '-') << " ";
            }
            std::cout << std::endl;
        }

        // Test empty/full
        std::cout << "empty(): " << p.empty() << std::endl;
        std::cout << "full(): " << p.full() << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "Errore: " << e.what() << std::endl;
    }
}

void debug_add_failure() {
    std::cout << "=== DEBUG ADD FAILURE ===" << std::endl;

    tetris game(8, 6);
    piece p;
    // CATTURA L'ECCEZIONE!
    try {
        std::stringstream ss("4 150 (()())");

        ss >> p;  // Dovrebbe lanciare eccezione

        std::cout << "✗ ERRORE: Eccezione NON lanciata per input invalido!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✓ OK: Eccezione catturata: " << e.what() << std::endl;
        return;
    }

    std::cout << "Pezzo parsed: side=" << p.side()
              << ", empty=" << p.empty() << ", full=" << p.full() << std::endl;

    // Coordinate dal test: (3,4)
    int x = 3, y = 4;

    std::cout << "Test containment(" << x << "," << y << "): "
              << game.containment(p, x, y) << std::endl;

    try {
        game.add(p, x, y);
        std::cout << "✓ add riuscito" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ add fallito: " << e.what() << std::endl;

        // Debug: perché containment fallisce?
        std::cout << "Debug containment:" << std::endl;
        for (uint32_t i = 0; i < p.side(); i++) {
            for (uint32_t j = 0; j < p.side(); j++) {
                if (p(i,j)) {
                    int field_x = x + j;
                    int field_y = y + i;
                    std::cout << "  Cell (" << i << "," << j << ") -> ("
                              << field_x << "," << field_y << ")";
                    if (field_x < 0 || field_x >= 8 || field_y < 0 || field_y >= 6) {
                        std::cout << " -> OUT OF BOUNDS" << std::endl;
                    } else {
                        std::cout << " -> OK" << std::endl;
                    }
                }
            }
        }
    }
}

void test_whitespace_parsing() {
    std::cout << "=== TEST PARSER CON SPAZI BIANCHI ===" << std::endl;

    // Test 1: Spazi normali
    std::string test1 = "4 75 ( () () [ ] [ ] )";
    std::stringstream ss1(test1);
    piece p1;

    try {
        ss1 >> p1;
        std::cout << "✓ Test 1 (spazi semplici): OK" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ Test 1: " << e.what() << std::endl;
    }

    // Test 2: Spazi complessi come nell'esempio
    std::string test2 =
        "4 \t75  \n"
        "  ( (\n"
        " [ ]()[\n"
        " ]()) (( )[\n"
        " ]()[])([](\n"
        " )()(  )) ( ()[](\n"
        "  )() ) )";

    std::stringstream ss2(test2);
    piece p2;

    try {
        ss2 >> p2;
        std::cout << "✓ Test 2 (spazi complessi): OK" << std::endl;
        std::cout << "  Output: " << p2 << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ Test 2: " << e.what() << std::endl;
    }

    // Test 3: Newline ovunque
    std::string test3 = "2\n100\n(\n)\n";
    std::stringstream ss3(test3);
    piece p3;

    try {
        ss3 >> p3;
        std::cout << "✓ Test 3 (newline): OK" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ Test 3: " << e.what() << std::endl;
    }
}

void test_height_1() {
    std::cout << "=== TEST m_height = 1 ===" << std::endl;

    tetris game(4, 1); // Campo altissimo 1!

    // Pezzo 1x1 - dovrebbe funzionare
    piece p1(1, 100);
    p1(0,0) = true;

    try {
        game.insert(p1, 1);
        std::cout << "✓ Pezzo 1x1 in campo 1 alto: OK" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✗ Pezzo 1x1: " << e.what() << std::endl;
    }

    // Pezzo 2x2 - IMPOSSIBILE!
    piece p2(2, 200);
    p2(0,0) = true; p2(0,1) = true;
    p2(1,0) = true; p2(1,1) = true;

    try {
        game.insert(p2, 0);
        std::cout << "✗ Pezzo 2x2 NON bloccato!" << std::endl;
    } catch (const tetris_exception& e) {
        std::cout << "✓ Pezzo 2x2 bloccato (troppo alto): " << e.what() << std::endl;
    }
}

void comprehensive_cut_row_test() {
    std::cout << "=== TEST CUT_ROW COMPLETO ===" << std::endl;

    // Test 1: Pezzo 4x4, cut riga 1
    piece p1(4, 100);
    // Pattern noto
    p1(0,0)=1; p1(0,1)=1; p1(0,2)=0; p1(0,3)=0;
    p1(1,0)=1; p1(1,1)=0; p1(1,2)=1; p1(1,3)=0;
    p1(2,0)=0; p1(2,1)=1; p1(2,2)=0; p1(2,3)=1;
    p1(3,0)=0; p1(3,1)=0; p1(3,2)=1; p1(3,3)=1;

    std::cout << "Prima di cut_row(1):" << std::endl;
    p1.print_ascii_art(std::cout);

    piece backup1 = p1;
    p1.cut_row(1);

    std::cout << "Dopo cut_row(1):" << std::endl;
    p1.print_ascii_art(std::cout);

    // Verifica manuale
    bool ok1 = true;
    // Dopo cut_row(1), riga 1 diventa vecchia riga 2, etc.
    for(uint32_t c = 0; c < 4; c++) {
        if (p1(1,c) != backup1(2,c)) ok1 = false;
        if (p1(2,c) != backup1(3,c)) ok1 = false;
        if (p1(3,c) != false) ok1 = false; // Ultima riga vuota
    }
    std::cout << (ok1 ? "✓ cut_row(1) OK" : "✗ cut_row(1) ERRATO") << std::endl;

    // Test 2: Pezzo 2x2, cut riga 0
    piece p2(2, 200);
    p2(0,0)=1; p2(0,1)=0;
    p2(1,0)=0; p2(1,1)=1;

    p2.cut_row(0);

    bool ok2 = (p2(0,0)==false && p2(0,1)==true &&  // Vecchia riga 1
                p2(1,0)==false && p2(1,1)==false); // Nuova riga vuota
    std::cout << (ok2 ? "✓ cut_row(0) 2x2 OK" : "✗ cut_row(0) 2x2 ERRATO") << std::endl;

    // Test 3: Eccezione per indice fuori range
    piece p3(2, 300);
    bool exception_thrown = false;
    try {
        p3.cut_row(2); // Fuori range
    } catch (const tetris_exception& e) {
        exception_thrown = true;
    }
    std::cout << (exception_thrown ? "✓ cut_row(2) bloccato" : "✗ cut_row(2) NON bloccato") << std::endl;

    // Test 4: Pezzo diventa vuoto dopo cut_row
    piece p4(2, 400);
    p4(0,0)=1; p4(0,1)=1; // Solo prima riga piena
    // Seconda riga già vuota

    p4.cut_row(0);
    std::cout << (p4.empty() ? "✓ Pezzo diventato vuoto dopo cut_row"
                             : "✗ Pezzo NON vuoto dopo cut_row") << std::endl;
}

void debug_cut_row_invalid_piece() {
    std::cout << "=== DEBUG CUT_ROW INVALID PIECE ===" << std::endl;

    // Prova pattern che potrebbero causare problemi
    piece p(4, 100);

    // Pattern 1: Solo alcune celle piene sparse
    p(0,0)=1; p(0,2)=1;
    p(1,1)=1;
    p(2,2)=1; p(2,3)=1;
    p(3,0)=1; p(3,1)=1;

    std::cout << "Prima di cut_row(2):" << std::endl;
    p.print_ascii_art(std::cout);

    // Salva stato
    uint32_t old_side = p.side();
    int old_color = p.color();
    bool old_empty = p.empty();
    bool old_full = p.full();

    try {
        p.cut_row(2);

        std::cout << "Dopo cut_row(2):" << std::endl;
        p.print_ascii_art(std::cout);

        // Verifica consistenza
        std::cout << "\nVerifica consistenza:" << std::endl;
        std::cout << "side: " << p.side() << " (era " << old_side << ")"
                  << (p.side() == old_side ? " ✅" : " ❌") << std::endl;
        std::cout << "color: " << p.color() << " (era " << old_color << ")"
                  << (p.color() == old_color ? " ✅" : " ❌") << std::endl;
        std::cout << "empty: " << p.empty() << " (era " << old_empty << ")" << std::endl;
        std::cout << "full: " << p.full() << " (era " << old_full << ")" << std::endl;

        // Verifica accesso a tutte le celle
        bool access_ok = true;
        for (uint32_t i = 0; i < p.side(); i++) {
            for (uint32_t j = 0; j < p.side(); j++) {
                try {
                    bool val = p(i,j); // Solo accesso
                    (void)val; // Ignora
                } catch (...) {
                    access_ok = false;
                    std::cout << "Accesso fallito a (" << i << "," << j << ")" << std::endl;
                }
            }
        }
        std::cout << "Accesso a tutte le celle: " << (access_ok ? "✅" : "❌") << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

void test_cut_row_io_roundtrip() {
    std::cout << "=== TEST CUT_ROW + I/O ===" << std::endl;

    piece p(4, 150);
    // Pattern complesso
    p(0,0)=1; p(0,2)=1;
    p(1,1)=1; p(1,3)=1;
    p(2,0)=1; p(2,2)=1;
    p(3,1)=1; p(3,3)=1;

    std::cout << "Originale: " << p << std::endl;

    // Applica cut_row
    p.cut_row(1);

    std::cout << "Dopo cut_row(1): " << p << std::endl;

    // Prova I/O round-trip
    std::stringstream ss;
    ss << p;

    piece p2;
    try {
        ss >> p2;
        std::cout << "Dopo round-trip: " << p2 << std::endl;

        if (p == p2) {
            std::cout << "✅ Round-trip dopo cut_row: OK" << std::endl;
        } else {
            std::cout << "❌ Round-trip dopo cut_row: PEZZI DIVERSI!" << std::endl;
            std::cout << "   Forse D() produce output sbagliato dopo cut_row?" << std::endl;
        }
    } catch (const tetris_exception& e) {
        std::cout << "❌ Parsing dopo cut_row fallito: " << e.what() << std::endl;
    }
}
void test_with_whitespace() {
    std::cout << "\n=== TEST WITH WHITESPACE ===\n";

    std::string input =
        "  4 \t75  \n"
        "    ( (\n"
        " [ ]()[\n"
        " ]()) (( )[\n"
        " ]()[])([](\n"
        " )()(  )) ( ()[](\n"
        "  )() ) )";

    std::cout << "Input (with whitespace):\n" << input << "\n---\n";

    std::stringstream ss(input);
    piece p;

    try {
        ss >> p;
        std::cout << "✓ Parsed successfully\n";
        std::cout << "Side: " << p.side() << ", Color: " << p.color() << "\n";

        // Ora scrivi per vedere se l'output è corretto
        std::stringstream ss2;
        ss2 << p;
        std::string output = ss2.str();
        std::cout << "Output: " << output << "\n";

        // L'output dovrebbe essere senza spazi: "4 75 (([]()[]())(()[]()[])([]()()())(()[]()()))"
        // Controlliamo l'inizio
        if (output.substr(0, 6) == "4 75 ") {
            std::cout << "✓ Output starts correctly\n";

            // Conta le parentesi per verifica approssimativa
            int open_paren = std::count(output.begin(), output.end(), '(');
            int close_paren = std::count(output.begin(), output.end(), ')');
            int open_bracket = std::count(output.begin(), output.end(), '[');
            int close_bracket = std::count(output.begin(), output.end(), ']');

            std::cout << "Parentesi: (" << open_paren << ", )" << close_paren
                      << ", [" << open_bracket << ", ]" << close_bracket << "\n";

            if (open_paren == close_paren && open_bracket == close_bracket) {
                std::cout << "✓ Parentesi bilanciate\n";
            } else {
                std::cout << "✗ Parentesi NON bilanciate!\n";
            }
        } else {
            std::cout << "✗ Output doesn't match expected format\n";
        }

    } catch (const tetris_exception& e) {
        std::cout << "✗ Failed: " << e.what() << "\n";

        // Mostra stato dello stream
        std::cout << "Stream state: fail=" << ss.fail()
                  << ", eof=" << ss.eof() << ", bad=" << ss.bad() << "\n";
    }
}
void test_cut_row_spec_example() {
    std::cout << "=== TEST CUT_ROW ESEMPIO SPECIFICA ===" << std::endl;

    piece p(4, 100);
    // Pattern dall'esempio grafico
    p(0,0)=1; p(0,1)=0; p(0,2)=1; p(0,3)=0;  // X-X-
    p(1,0)=0; p(1,1)=1; p(1,2)=0; p(1,3)=1;  // -X-X ← Tagliare questa
    p(2,0)=1; p(2,1)=0; p(2,2)=1; p(2,3)=0;  // X-X-
    p(3,0)=0; p(3,1)=1; p(3,2)=0; p(3,3)=1;  // -X-X

    std::cout << "Prima di cut_row(1):" << std::endl;
    for(uint32_t r = 0; r < 4; r++) {
        for(uint32_t c = 0; c < 4; c++) {
            std::cout << (p(r,c) ? 'X' : '-') << ' ';
        }
        std::cout << std::endl;
    }

    p.cut_row(1);

    std::cout << "\nDopo cut_row(1):" << std::endl;
    for(uint32_t r = 0; r < 4; r++) {
        for(uint32_t c = 0; c < 4; c++) {
            std::cout << (p(r,c) ? 'X' : '-') << ' ';
        }
        std::cout << std::endl;
    }

    // Secondo la specifica: cell (0,2) "shifts" down to (1,2)
    // Quindi la X a (0,2) dovrebbe finire a (1,2)
    bool correct = (p(1,2) == true);  // Vecchia (0,2) ora a (1,2)
    std::cout << "\ncell (0,2) -> (1,2): " << (correct ? "✅ CORRETTO" : "❌ SBAGLIATO") << std::endl;
}

void test_tetris_game_session() {
    std::cout << "\n=== INIZIO SESSIONE DI GIOCO TETRIS ===\n";

    // 1. Crea campo di gioco 10x20
    tetris game(10, 20);
    std::cout << "Creato campo 10x20\n";
    std::cout << "Score iniziale: " << game.score() << "\n";

    // 2. Crea alcuni pezzi Tetris classici
    // Pezzo I (4x1)
    piece i_piece(4, 201);  // Cyan
    for (uint32_t j = 0; j < 4; ++j) {
        i_piece(1, j) = true;  // Riga centrale piena
    }

    // Pezzo O (2x2)
    piece o_piece(2, 226);  // Giallo
    for (uint32_t i = 0; i < 2; ++i) {
        for (uint32_t j = 0; j < 2; ++j) {
            o_piece(i, j) = true;
        }
    }

    // Pezzo T (3x3)
    piece t_piece(4, 129);  // Viola
    t_piece(0, 1) = true;
    t_piece(1, 0) = true;
    t_piece(1, 1) = true;
    t_piece(1, 2) = true;

    // 3. Inserisci pezzi
    std::cout << "\n--- Inserimento pezzo I al centro ---\n";
    try {
        game.insert(i_piece, 3);  // x=3 (centro)
        std::cout << "Inserito pezzo I a x=3\n";
        std::cout << "Score: " << game.score() << "\n";
    } catch (const tetris_exception& e) {
        std::cout << "ERRORE: " << e.what() << "\n";
    }
    piece full_row_piece(16, 46);
    for (uint32_t j = 0; j < 10; ++j) {
        full_row_piece(0, j) = true;  // Riga 0 completamente piena
    }

    try {
        game.insert(full_row_piece, 0);
        std::cout << "Inserita riga COMPLETAMENTE piena in fondo!\n";
        std::cout << "Score: " << game.score() << " (dovrebbe essere 10)\n";
    } catch (...) {
        std::cout << "Non inserito\n";
    }
    std::cout << "\n--- Inserimento pezzo O a sinistra ---\n";
    try {
        game.insert(o_piece, 0);  // x=0 (sinistra)
        std::cout << "Inserito pezzo O a x=0\n";
        std::cout << "Score: " << game.score() << "\n";
    } catch (const tetris_exception& e) {
        std::cout << "ERRORE: " << e.what() << "\n";
    }

    std::cout << "\n--- Inserimento pezzo T a destra ---\n";
    try {
        game.insert(t_piece, 6);  // x=6 (destra)
        std::cout << "Inserito pezzo T a x=6\n";
        std::cout << "Score: " << game.score() << "\n";
    } catch (const tetris_exception& e) {
        std::cout << "ERRORE: " << e.what() << "\n";
    }

    // 4. Prova a creare una riga completa
    std::cout << "\n--- Tentativo di creare riga completa ---\n";

    // Crea un pezzo lungo 10 (l'intera larghezza)
    piece line_piece(2, 46);  // 16x16 - abbastanza grande

    // Oppure, se vuoi mantenere potenza di 2 ma più piccolo, usa 8:
    // piece line_piece(8, 46);  // 8x8 - ma allora solo 8 colonne!

    // Riempi le prime 10 colonne della riga 1
    for (uint32_t j = 0; j < line_piece.side(); ++j) {
        line_piece(1, j) = true;
    }

    try {
        // Inserisci in fondo (y dovrebbe essere 0)
        game.insert(line_piece, 0);
        std::cout << "Inserita linea completa in fondo!\n";
        std::cout << "Score dopo riga completa: " << game.score() << "\n";
        std::cout << "Dovrebbe essere aumentato di " << game.width() << " = "
                  << (game.score() - 0) << "\n";
    } catch (const tetris_exception& e) {
        std::cout << "ERRORE: " << e.what() << "\n";
    }

    // 5. Test I/O: salva e ricarica
    std::cout << "\n--- Test salvataggio/ricarica ---\n";
    std::stringstream ss;
    ss << game;
    std::cout << "Game salvato in stringa di " << ss.str().size() << " bytes\n";

    tetris loaded_game;
    try {
        ss >> loaded_game;
        std::cout << "Game ricaricato con successo!\n";
        std::cout << "Score originale: " << game.score()
                  << ", Score ricaricato: " << loaded_game.score() << "\n";

        // Verifica uguaglianza
        if (game == loaded_game) {
            std::cout << "✓ I due giochi sono identici!\n";
        } else {
            std::cout << "✗ I due giochi sono DIVERSI!\n";
        }
    } catch (const tetris_exception& e) {
        std::cout << "ERRORE nel caricamento: " << e.what() << "\n";
    }

    // 6. Test iteratori
    std::cout << "\n--- Test iteratori ---\n";
    int piece_count = 0;
    for (auto it = game.begin(); it != game.end(); ++it) {
        std::cout << "Pezzo " << piece_count++ << ": "
                  << "size=" << it->p.side()
                  << ", pos=(" << it->x << "," << it->y << ")\n";
    }
    std::cout << "Totale pezzi sul campo: " << piece_count << "\n";

    // 7. Test move operations
    std::cout << "\n--- Test move operations ---\n";
    tetris game2 = std::move(game);
    std::cout << "Dopo move constructor:\n";
    std::cout << "  game2 score: " << game2.score() << "\n";
    std::cout << "  game2 pieces: ";
    int count2 = 0;
    for (auto it = game2.begin(); it != game2.end(); ++it) count2++;
    std::cout << count2 << "\n";

    // 8. Test edge cases
    std::cout << "\n--- Test edge cases ---\n";

    // Prova a inserire fuori dai bounds
    std::cout << "Test inserimento fuori bounds (x negativo): ";
    try {
        piece small(2, 100);
        small(0,0) = small(0,1) = small(1,0) = small(1,1) = true;
        game2.insert(small, -5);
        std::cout << "INSERITO (strano!)\n";
    } catch (const tetris_exception& e) {
        std::cout << "BLOCCATO: " << e.what() << "\n";
    }

    // Prova pezzo vuoto
    std::cout << "Test inserimento pezzo vuoto: ";
    piece empty_piece(2, 50);  // Tutto false
    try {
        game2.insert(empty_piece, 5);
        std::cout << "INSERITO\n";
    } catch (const tetris_exception& e) {
        std::cout << "ERRORE: " << e.what() << "\n";
    }

    std::cout << "\n=== FINE SESSIONE DI GIOCO ===\n";
    std::cout << "Score finale: " << game2.score() << "\n";

    // 9. Valgrind check
    std::cout << "\n[NOTA] Esegui con: valgrind --leak-check=full ./programma\n";
    std::cout << "       per verificare memory leaks\n";
}

void test_tetris_io() {
    std::cout << "=== TEST TETRIS I/O ===" << std::endl;

    // 1. Crea un tetris con alcuni pezzi
    tetris game1(8, 6);

    piece p1(2, 100);  // Pezzo 2x2 pieno
    p1(0,0) = p1(0,1) = p1(1,0) = p1(1,1) = true;

    piece p2(2, 200);  // Altro pezzo 2x2
    p2(0,0) = p2(0,1) = p2(1,0) = p2(1,1) = true;

    // Aggiungi in ordine: prima p1, poi p2
    game1.add(p1, 0, 0);
    game1.add(p2, 2, 2);

    std::cout << "Game1 score: " << game1.score() << std::endl;
    std::cout << "Game1 pieces: ";
    for (auto it = game1.begin(); it != game1.end(); ++it) {
        std::cout << "(" << it->x << "," << it->y << ") ";
    }
    std::cout << std::endl;

    // 2. Salva in stringstream
    std::stringstream ss;
    ss << game1;

    std::cout << "Saved to stream: " << ss.str() << std::endl;

    // 3. Ricarica
    tetris game2;
    try {
        ss >> game2;
        std::cout << "✓ Load successful!" << std::endl;

        std::cout << "Game2 score: " << game2.score() << std::endl;
        std::cout << "Game2 pieces: ";
        for (auto it = game2.begin(); it != game2.end(); ++it) {
            std::cout << "(" << it->x << "," << it->y << ") ";
        }
        std::cout << std::endl;

        // 4. Verifica uguaglianza
        if (game1 == game2) {
            std::cout << "✓ Games are EQUAL!" << std::endl;
        } else {
            std::cout << "✗ Games are DIFFERENT!" << std::endl;

            // Debug: confronta dettagli
            std::cout << "  Game1 width/height/score: "
                      << game1.width() << "/" << game1.height() << "/" << game1.score() << std::endl;
            std::cout << "  Game2 width/height/score: "
                      << game2.width() << "/" << game2.height() << "/" << game2.score() << std::endl;
        }

    } catch (const tetris_exception& e) {
        std::cout << "✗ Load failed: " << e.what() << std::endl;
    }
}
void test_insert_basic() {
    std::cout << "=== TEST INSERT BASIC ===" << std::endl;

    tetris game(8, 6);  // campo 8x6

    piece p(2, 100);  // pezzo 2x2 pieno
    p(0,0) = p(0,1) = p(1,0) = p(1,1) = true;

    try {
        // Inserisci al centro
        game.insert(p, 3);
        std::cout << "✓ Insert at x=3 successful" << std::endl;
        std::cout << "  Piece count: ";
        int count = 0;
        for (auto it = game.begin(); it != game.end(); ++it) {
            std::cout << "(" << it->x << "," << it->y << ") ";
            count++;
        }
        std::cout << "[" << count << " pieces]" << std::endl;

        // Inserisci un altro sopra
        game.insert(p, 3);
        std::cout << "✓ Second insert at same x successful" << std::endl;

        // Prova a inserire dove non c'è spazio
        for (int i = 0; i < 5; i++) {
            game.insert(p, 5);
        }
        std::cout << "✓ Multiple inserts successful" << std::endl;

        std::cout << "Final score: " << game.score() << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "✗ Insert failed: " << e.what() << std::endl;
    }
}
void test_insert_detailed() {
    std::cout << "\n=== TEST INSERT DETAILED ===" << std::endl;

    tetris game(8, 6);
    std::cout << "Field: " << game.width() << "x" << game.height() << std::endl;

    piece p(2, 100);  // pezzo 2x2 pieno
    p(0,0) = p(0,1) = p(1,0) = p(1,1) = true;

    std::cout << "Piece side: " << p.side() << std::endl;

    for (int i = 1; i <= 5; i++) {
        try {
            std::cout << "\nInsert " << i << " at x=3..." << std::endl;
            game.insert(p, 3);

            // Trova l'ultimo pezzo inserito (è in testa alla lista)
            auto it = game.begin();
            std::cout << "  Success! Piece at (" << it->x << "," << it->y << ")" << std::endl;

            // Mostra tutte le posizioni
            std::cout << "  All pieces: ";
            int count = 0;
            for (auto it2 = game.begin(); it2 != game.end(); ++it2) {
                std::cout << "(" << it2->x << "," << it2->y << ") ";
                count++;
            }
            std::cout << "[" << count << " total]" << std::endl;

            // Calcola quale y sarebbe la prossima
            int next_y = -1;
            for (int y = game.height() - 1; y >= 0; y--) {
                if (game.containment(p, 3, y)) {
                    next_y = y;
                    break;
                }
            }
            std::cout << "  Next possible y: " << next_y << std::endl;

        } catch (const tetris_exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            break;
        }
    }

    std::cout << "\nFinal score: " << game.score() << std::endl;
}
void test_parsing_and_rotation() {
    std::cout << "\n=== TEST PARSING AND ROTATION ===" << std::endl;

    // Test alcuni formati ricorsivi tipici
    struct TestCase {
        std::string input;
        std::string description;
    };

    TestCase tests[] = {
        {"2 100 ()", "2x2 pieno"},
        {"2 100 []", "2x2 vuoto"},
        {"4 100 (()()()())", "4x4 con 4 quadranti pieni"},
        {"4 100 ([][][][])", "4x4 con 4 quadranti vuoti"},
        {"4 100 (([]()[]())(()[]()[])([]()()())(()[]()()))", "Forma specifica"},
    };

    for (const auto& test : tests) {
        std::cout << "\nTest: " << test.description << std::endl;
        std::cout << "Input: " << test.input << std::endl;

        std::stringstream ss(test.input);
        piece p;

        try {
            ss >> p;
            std::cout << "  ✓ Parsing successful" << std::endl;
            std::cout << "  Side: " << p.side() << ", Color: " << p.color() << std::endl;
            std::cout << "  Empty: " << p.empty() << ", Full: " << p.full() << std::endl;

            // Prova rotazione
            piece p_rotated = p;
            p_rotated.rotate();

            // Verifica che non sia cambiato size/empty/full
            if (p_rotated.side() != p.side()) {
                std::cout << "  ✗ Rotation changed side!" << std::endl;
            }
            if (p_rotated.empty() != p.empty()) {
                std::cout << "  ✗ Rotation changed empty status!" << std::endl;
            }
            if (p_rotated.full() != p.full()) {
                std::cout << "  ✗ Rotation changed full status!" << std::endl;
            }

        } catch (const tetris_exception& e) {
            std::cout << "  ✗ Parsing failed: " << e.what() << std::endl;
        }
    }

    // Test rotazione specifica
    std::cout << "\n=== SPECIFIC ROTATION TEST ===" << std::endl;
    {
        // Crea pezzo a L 4x4
        piece p(4, 100);
        // Forma a L:
        // X...
        // X...
        // X...
        // XX..
        p(0,0) = true;
        p(1,0) = true;
        p(2,0) = true;
        p(3,0) = true;
        p(3,1) = true;

        std::cout << "Original L shape 4x4:" << std::endl;
        for (uint32_t i = 0; i < p.side(); i++) {
            for (uint32_t j = 0; j < p.side(); j++) {
                std::cout << (p(i,j) ? 'X' : '.');
            }
            std::cout << std::endl;
        }

        piece p1 = p;
        p1.rotate(); // 90°
        std::cout << "\nAfter 90° rotation:" << std::endl;
        for (uint32_t i = 0; i < p1.side(); i++) {
            for (uint32_t j = 0; j < p1.side(); j++) {
                std::cout << (p1(i,j) ? 'X' : '.');
            }
            std::cout << std::endl;
        }

        piece p2 = p1;
        p2.rotate(); // 180° (rispetto all'originale)
        std::cout << "\nAfter 180° rotation:" << std::endl;
        for (uint32_t i = 0; i < p2.side(); i++) {
            for (uint32_t j = 0; j < p2.side(); j++) {
                std::cout << (p2(i,j) ? 'X' : '.');
            }
            std::cout << std::endl;
        }

        // Verifica che 4 rotazioni tornino all'originale
        piece p4 = p;
        for (int i = 0; i < 4; i++) {
            p4.rotate();
        }

        if (p4 == p) {
            std::cout << "✓ 4 rotations return to original" << std::endl;
        } else {
            std::cout << "✗ 4 rotations do NOT return to original!" << std::endl;
        }
    }
}
void test_piece_roundtrip() {
    std::cout << "\n=== TEST PIECE ROUND-TRIP ===" << std::endl;

    // Test vari pezzi
    piece p1(2, 100);
    p1(0,0) = p1(0,1) = p1(1,0) = true;  // Forma a L 2x2

    piece p2(4, 150);
    // Riempi metà del pezzo
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 2; j++) {
            p2(i,j) = true;
        }
    }

    piece tests[] = {p1, p2};

    for (int i = 0; i < 2; i++) {
        std::cout << "\nTest piece " << (i+1) << ":" << std::endl;

        // Scrivi
        std::stringstream ss;
        ss << tests[i];
        std::string written = ss.str();
        std::cout << "  Written: " << written << std::endl;

        // Leggi
        ss.seekg(0);
        piece read_piece;
        try {
            ss >> read_piece;

            // Compara
            if (tests[i] == read_piece) {
                std::cout << "  ✓ Round-trip successful" << std::endl;
                std::cout << "  Read: " << read_piece.side() << " " << read_piece.color() << " ..." << std::endl;
            } else {
                std::cout << "  ✗ Round-trip FAILED - pieces are different!" << std::endl;

                // Debug: mostra differenze
                std::cout << "  Original: side=" << tests[i].side() << ", color=" << tests[i].color() << std::endl;
                std::cout << "  Read: side=" << read_piece.side() << ", color=" << read_piece.color() << std::endl;

                if (tests[i].side() != read_piece.side()) {
                    std::cout << "    Sides differ!" << std::endl;
                }
                if (tests[i].color() != read_piece.color()) {
                    std::cout << "    Colors differ!" << std::endl;
                }
            }

        } catch (const tetris_exception& e) {
            std::cout << "  ✗ Reading failed: " << e.what() << std::endl;
        }
    }
}

void test_empty_full_quadrants() {
    std::cout << "\n=== TEST EMPTY/FULL QUADRANTS ===" << std::endl;

    // Crea pezzo 2x2 con forma a L
    piece p(2, 100);
    p(0,0) = true;   // TL
    p(1,0) = true;   // BL
    p(1,1) = false;  // BR
    p(0,1) = false;  // TR

    std::cout << "Piece 2x2 L shape:" << std::endl;
    for (uint32_t i = 0; i < 2; i++) {
        for (uint32_t j = 0; j < 2; j++) {
            std::cout << (p(i,j) ? 'X' : '.');
        }
        std::cout << std::endl;
    }

    std::cout << "\nQuadrant tests:" << std::endl;
    std::cout << "Whole piece empty: " << p.empty() << " (should be 0)" << std::endl;
    std::cout << "Whole piece full: " << p.full() << " (should be 0)" << std::endl;
    std::cout << "TL quadrant (0,0,1): empty=" << p.empty(0,0,1) << ", full=" << p.full(0,0,1) << " (should be 0,1)" << std::endl;
    std::cout << "TR quadrant (0,1,1): empty=" << p.empty(0,1,1) << ", full=" << p.full(0,1,1) << " (should be 1,0)" << std::endl;
    std::cout << "BL quadrant (1,0,1): empty=" << p.empty(1,0,1) << ", full=" << p.full(1,0,1) << " (should be 0,1)" << std::endl;
    std::cout << "BR quadrant (1,1,1): empty=" << p.empty(1,1,1) << ", full=" << p.full(1,1,1) << " (should be 1,0)" << std::endl;

    // Test output
    std::stringstream ss;
    ss << p;
    std::cout << "\nOutput: " << ss.str() << std::endl;
    std::cout << "Expected something like: 2 100 (()[][]()) or similar" << std::endl;

    // Verifica round-trip
    ss.seekg(0);
    piece p2;
    ss >> p2;

    if (p == p2) {
        std::cout << "✓ Round-trip successful" << std::endl;
    } else {
        std::cout << "✗ Round-trip FAILED" << std::endl;
    }
}
void simulate_play_game_test() {
    std::cout << "\n=== SIMULATE PLAY GAME TEST ===" << std::endl;

    // Simula ciò che probabilmente fa il test:
    // 1. Legge pezzi da stream
    // 2. Li ruota
    // 3. Li inserisce
    // 4. Confronta lo stato finale

    // Esempio con un pezzo semplice
    std::stringstream input_stream;
    input_stream << "2 100 ()\n";  // Pezzo 2x2 pieno

    tetris game(8, 6);

    try {
        // Leggi pezzo
        piece p;
        input_stream >> p;
        std::cout << "Read piece: side=" << p.side() << ", color=" << p.color() << std::endl;

        // Ruota (se richiesto dal test)
        p.rotate();
        std::cout << "Rotated piece" << std::endl;

        // Inserisci
        game.insert(p, 3);  // Inserisci al centro
        std::cout << "Inserted at x=3" << std::endl;

        // Mostra stato
        std::cout << "Game state:" << std::endl;
        std::cout << "Score: " << game.score() << std::endl;
        std::cout << "Pieces: ";
        for (auto it = game.begin(); it != game.end(); ++it) {
            std::cout << "(" << it->x << "," << it->y << ") ";
        }
        std::cout << std::endl;

        // Scrivi output per confronto
        std::stringstream output_stream;
        output_stream << game;
        std::cout << "Output:\n" << output_stream.str() << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}
void test_read_complete_game() {
    std::cout << "\n=== TEST READ COMPLETE GAME ===" << std::endl;

    // Simula un file di gioco salvato
    std::string game_data =
        "10 8 6\n"  // score=10, width=8, height=6
        "2 100 () 3 4\n"
        "2 200 () 1 2\n"
        "4 150 (()()()[]) 0 0\n";

    std::stringstream ss(game_data);
    tetris game;

    try {
        ss >> game;
        std::cout << "✓ Game loaded successfully" << std::endl;
        std::cout << "Score: " << game.score() << " (expected: 10)" << std::endl;
        std::cout << "Size: " << game.width() << "x" << game.height() << std::endl;

        std::cout << "Pieces:" << std::endl;
        int count = 0;
        for (auto it = game.begin(); it != game.end(); ++it) {
            std::cout << "  " << count << ": side=" << it->p.side()
                      << ", color=" << it->p.color()
                      << ", pos=(" << it->x << "," << it->y << ")" << std::endl;
            count++;
        }

        // Ora riscrivi e confronta
        std::stringstream ss2;
        ss2 << game;
        std::string rewritten = ss2.str();

        std::cout << "\nRewritten game:" << std::endl;
        std::cout << rewritten;

        // Prova a rileggere
        ss2.seekg(0);
        tetris game2;
        ss2 >> game2;

        if (game == game2) {
            std::cout << "\n✓ Round-trip successful" << std::endl;
        } else {
            std::cout << "\n✗ Round-trip FAILED" << std::endl;
        }

    } catch (const tetris_exception& e) {
        std::cout << "✗ Error loading game: " << e.what() << std::endl;
    }
}
void debug_containment() {
    std::cout << "\n=== DEBUG CONTAINMENT ===\n";

    tetris game(8, 6);
    game.add(piece(2, 100), 3, 4);  // Simula primo pezzo
    game.add(piece(2, 200), 1, 2);  // Simula secondo pezzo

    // Prova a parsare il terzo pezzo
    std::stringstream ss("4 150 (()()()[])");
    piece p;
    ss >> p;

    std::cout << "Piece parsed: side=" << p.side() << std::endl;
    std::cout << "Shape at (0,0):\n";
    for (int i = p.side()-1; i >= 0; i--) {  // Dal basso verso l'alto
        for (uint32_t j = 0; j < p.side(); j++) {
            std::cout << (p(i,j) ? 'X' : '.');
        }
        std::cout << std::endl;
    }

    std::cout << "\nTrying containment at (0,0)..." << std::endl;
    bool result = game.containment(p, 0, 0);
    std::cout << "Result: " << result << std::endl;

    if (!result) {
        std::cout << "\nChecking why containment fails:\n";

        // Controlla bounds
        for (uint32_t i = 0; i < p.side(); ++i) {
            for (uint32_t j = 0; j < p.side(); ++j) {
                if (p(i, j)) {
                    int field_x = 0 + static_cast<int>(j);
                    int field_y = 0 + static_cast<int>(p.side() - 1 - i);

                    std::cout << "  Cell (" << i << "," << j << ") -> ("
                              << field_x << "," << field_y << "): ";

                    if (field_y < 0 || field_y >= 6) {
                        std::cout << "Y OUT OF BOUNDS!\n";
                    } else if (field_x < 0 || field_x >= 8) {
                        std::cout << "X OUT OF BOUNDS!\n";
                    } else {
                        std::cout << "OK bounds\n";
                    }
                }
            }
        }

        // Controlla collisioni
        std::cout << "\nChecking collisions with existing pieces:\n";
        int piece_num = 0;
        for (auto it = game.begin(); it != game.end(); ++it, ++piece_num) {
            std::cout << "  With piece " << piece_num << " at ("
                      << it->x << "," << it->y << "):\n";

            // Logica simile a containment per debug
        }
    }
}
void test_add_order() {
    std::cout << "\n=== TEST ADD ORDER ===\n";

    tetris game(8, 6);

    piece p1(1, 100); p1(0,0) = true;
    piece p2(1, 200); p2(0,0) = true;
    piece p3(1, 300); p3(0,0) = true;

    // Aggiungi nell'ordine p1, p2, p3
    game.add(p1, 0, 0);
    game.add(p2, 1, 0);
    game.add(p3, 2, 0);

    std::cout << "After adding p1, p2, p3 (in this order):\n";
    std::cout << "List order (from head): ";
    for (auto it = game.begin(); it != game.end(); ++it) {
        std::cout << it->p.color() << " ";
    }
    std::cout << "\nExpected: 300 200 100 (inverse of insertion order)\n";

    // Scrivi e vedi l'output
    std::stringstream ss;
    ss << game;
    std::cout << "\nOutput:\n" << ss.str();

    // Ora leggi e verifica
    ss.seekg(0);
    tetris game2;
    ss >> game2;

    std::cout << "\nAfter reading back:\n";
    std::cout << "List order: ";
    for (auto it = game2.begin(); it != game2.end(); ++it) {
        std::cout << it->p.color() << " ";
    }
    std::cout << std::endl;

    if (game == game2) {
        std::cout << "✓ Games are equal!\n";
    } else {
        std::cout << "✗ Games are different!\n";
    }
}
void reproduce_play_game_error() {
    std::cout << "\n=== REPRODUCE PLAY GAME ERROR ===\n";

    // Crea un gioco come nei test
    tetris game(10, 20);  // Dimensioni tipiche Tetris

    // Aggiungi alcuni pezzi (come nei test)
    std::string pieces[] = {
        "4 1 ((()()()())()()()())",  // Esempio dalla specifica
        "2 2 ()",
        "4 3 ([][](()())())"
    };

    int positions[][2] = {{3,0}, {5,0}, {0,0}};

    for (int i = 0; i < 3; i++) {
        std::stringstream ss(pieces[i]);
        piece p;
        ss >> p;

        // Ruota (come nei test)
        p.rotate();

        // Inserisci
        try {
            game.insert(p, positions[i][0]);
            std::cout << "✓ Inserted piece " << i << std::endl;
        } catch (const tetris_exception& e) {
            std::cout << "✗ Failed to insert piece " << i << ": " << e.what() << std::endl;
        }
    }

    // Ora salva
    std::stringstream saved;
    saved << game;
    std::cout << "\nSaved game:\n" << saved.str() << std::endl;

    // Prova a ricaricare
    saved.seekg(0);
    tetris loaded;
    try {
        saved >> loaded;
        std::cout << "✓ Game loaded back\n";

        if (game == loaded) {
            std::cout << "✓ Games are equal!\n";
        } else {
            std::cout << "✗ Games are NOT equal!\n";

            // Debug
            std::cout << "Original pieces: ";
            for (auto it = game.begin(); it != game.end(); ++it) std::cout << it->p.color() << " ";
            std::cout << "\nLoaded pieces: ";
            for (auto it = loaded.begin(); it != loaded.end(); ++it) std::cout << it->p.color() << " ";
            std::cout << std::endl;
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ Failed to load: " << e.what() << std::endl;
    }
}
void test_specific_failure() {
    std::cout << "\n=== TEST SPECIFIC FAILURE ===\n";

    // Test passo passo
    std::string test1 = "(()()()())";  // Solo TL
    std::string test2 = "()";          // Quadrante pieno singolo
    std::string test3 = "((()()()())()()()())";  // Completo

    std::cout << "Test 1 (TL only): " << test1 << std::endl;
    try {
        piece p(2, 1);  // 2x2 per test semplice
        std::stringstream ss(test1);
        C(ss, p, 0, 0, 2);
        std::cout << "✓ Test 1 passed\n";
    } catch (const tetris_exception& e) {
        std::cout << "✗ Test 1 failed: " << e.what() << std::endl;
    }

    std::cout << "\nTest 2 (single): " << test2 << std::endl;
    try {
        piece p(1, 1);
        std::stringstream ss(test2);
        C(ss, p, 0, 0, 1);
        std::cout << "✓ Test 2 passed\n";
    } catch (const tetris_exception& e) {
        std::cout << "✗ Test 2 failed: " << e.what() << std::endl;
    }

    std::cout << "\nTest 3 (full): " << test3 << std::endl;
    try {
        piece p(4, 1);
        std::stringstream ss(test3);
        C(ss, p, 0, 0, 4);
        std::cout << "✓ Test 3 passed\n";
    } catch (const tetris_exception& e) {
        std::cout << "✗ Test 3 failed: " << e.what() << std::endl;
    }
}
void test_2x2_full() {
    std::cout << "\n=== TEST 2x2 FULL ===\n";

    // Pezzo 2x2 pieno: "()"
    std::string test = "2 1 ()";
    std::cout << "Testing: " << test << std::endl;

    try {
        std::stringstream ss(test);
        piece p;
        ss >> p;
        std::cout << "✓ Parsed, side=" << p.side() << ", full=" << p.full() << "\n";
    } catch (const tetris_exception& e) {
        std::cout << "✗ Failed: " << e.what() << "\n";
    }

    // Pezzo 4x4 con 4 quadranti 2x2 pieni: "(()()()())"
    std::cout << "\nTesting 4x4: 4 1 (()()()())\n";
    try {
        std::stringstream ss("4 1 (()()()())");
        piece p;
        ss >> p;
        std::cout << "✓ Parsed, side=" << p.side() << ", full=" << p.full() << "\n";
    } catch (const tetris_exception& e) {
        std::cout << "✗ Failed: " << e.what() << "\n";
    }
}
void test_step_by_step() {
    std::cout << "\n=== TEST STEP BY STEP ===\n";

    std::string input = "((()()()())()()()())";
    std::cout << "Input: " << input << "\n";

    // Simuliamo manualmente ciò che C() dovrebbe fare
    std::stringstream ss(input);

    // Quadrante principale: inizia con '('
    char c = ss.get();
    std::cout << "1. Main start: '" << c << "' (should be '(')\n";

    // TL: inizia con '('
    c = ss.get();
    std::cout << "2. TL start: '" << c << "' (should be '(')\n";

    // TL è annidato: parsiamo i suoi 4 quadranti
    // TL di TL: "()"
    c = ss.get(); std::cout << "3. TL-TL '(': '" << c << "'\n";
    c = ss.get(); std::cout << "4. TL-TL ')': '" << c << "'\n";

    // TR di TL: "()"
    c = ss.get(); std::cout << "5. TL-TR '(': '" << c << "'\n";
    c = ss.get(); std::cout << "6. TL-TR ')': '" << c << "'\n";

    // BL di TL: "()"
    c = ss.get(); std::cout << "7. TL-BL '(': '" << c << "'\n";
    c = ss.get(); std::cout << "8. TL-BL ')': '" << c << "'\n";

    // BR di TL: "()"
    c = ss.get(); std::cout << "9. TL-BR '(': '" << c << "'\n";
    c = ss.get(); std::cout << "10. TL-BR ')': '" << c << "'\n";

    // Fine TL: ")"
    c = ss.get(); std::cout << "11. TL end: '" << c << "' (should be ')')\n";

    // TR: "()"
    c = ss.get(); std::cout << "12. TR '(': '" << c << "'\n";
    c = ss.get(); std::cout << "13. TR ')': '" << c << "'\n";

    // BL: "()"
    c = ss.get(); std::cout << "14. BL '(': '" << c << "'\n";
    c = ss.get(); std::cout << "15. BL ')': '" << c << "'\n";

    // BR: "()"
    c = ss.get(); std::cout << "16. BR '(': '" << c << "'\n";
    c = ss.get(); std::cout << "17. BR ')': '" << c << "'\n";

    // Fine quadrante principale: ")"
    c = ss.get(); std::cout << "18. Main end: '" << c << "' (should be ')')\n";

    // Dovrebbe essere EOF
    c = ss.get();
    if (ss) {
        std::cout << "19. Extra char: '" << c << "' (UNEXPECTED!)\n";
    } else {
        std::cout << "19. EOF (correct)\n";
    }
}
void test_rotate_and_insert() {
    std::cout << "\n=== TEST ROTATE AND INSERT ===\n";

    tetris game(10, 20);

    // Test con il pezzo dell'esempio
    std::string piece_str = "4 75 (([]()[]())(()[]()[])([]()()())(()[]()()))";
    std::stringstream ss(piece_str);
    piece p;
    ss >> p;

    std::cout << "Original piece:\n";
    for (uint32_t i = 0; i < p.side(); i++) {
        for (uint32_t j = 0; j < p.side(); j++) {
            std::cout << (p(i,j) ? 'X' : '.');
        }
        std::cout << std::endl;
    }

    // Ruota
    p.rotate();
    std::cout << "\nAfter rotation:\n";
    for (uint32_t i = 0; i < p.side(); i++) {
        for (uint32_t j = 0; j < p.side(); j++) {
            std::cout << (p(i,j) ? 'X' : '.');
        }
        std::cout << std::endl;
    }

    // Inserisci
    try {
        game.insert(p, 3);  // Inserisci al centro
        std::cout << "\n✓ Insert successful\n";
        std::cout << "Score: " << game.score() << "\n";
        std::cout << "Pieces in field: ";
        for (auto it = game.begin(); it != game.end(); ++it) {
            std::cout << "(" << it->x << "," << it->y << ") ";
        }
        std::cout << std::endl;

        // Prova a scrivere e rileggere
        std::stringstream saved;
        saved << game;
        std::cout << "\nSaved game:\n" << saved.str() << std::endl;

    } catch (const tetris_exception& e) {
        std::cout << "\n✗ Insert failed: " << e.what() << std::endl;
    }
}

void test_complete_roundtrip() {
    std::cout << "\n=== TEST COMPLETE ROUNDTRIP ===\n";

    // Crea un gioco complesso
    tetris game(8, 6);

    // Aggiungi diversi pezzi
    std::vector<std::string> piece_strings = {
        "2 100 ()",      // 2x2 pieno
        "4 150 (([]()[]())(()[]()[])([]()()())(()[]()()))",  // Forma complessa
        "2 200 []",      // 2x2 vuoto (non dovrebbe essere aggiunto se empty()=true)
    };

    int x_pos = 0;
    for (const auto& str : piece_strings) {
        std::stringstream ss(str);
        piece p;
        ss >> p;

        if (!p.empty()) {
            try {
                game.insert(p, x_pos);
                std::cout << "✓ Inserted piece at x=" << x_pos << std::endl;
                x_pos += 2;
            } catch (const tetris_exception& e) {
                std::cout << "✗ Failed to insert: " << e.what() << std::endl;
            }
        } else {
            std::cout << "Skipped empty piece\n";
        }
    }

    std::cout << "\nFinal game state:\n";
    std::cout << "Score: " << game.score() << std::endl;
    std::cout << "Pieces: ";
    for (auto it = game.begin(); it != game.end(); ++it) {
        std::cout << "(" << it->x << "," << it->y << ") ";
    }
    std::cout << std::endl;

    // Salva
    std::stringstream saved;
    saved << game;
    std::string saved_str = saved.str();
    std::cout << "\nSaved game:\n" << saved_str << std::endl;

    // Ricarica
    saved.seekg(0);
    tetris loaded;
    try {
        saved >> loaded;
        std::cout << "✓ Game loaded back\n";

        // Confronta
        if (game == loaded) {
            std::cout << "✓ Games are EQUAL!\n";
        } else {
            std::cout << "✗ Games are DIFFERENT!\n";

            // Debug dettagliato
            std::cout << "Original: " << game.width() << "x" << game.height()
                      << ", score=" << game.score() << ", pieces=";
            int count1 = 0;
            for (auto it = game.begin(); it != game.end(); ++it) count1++;
            std::cout << count1 << "\n";

            std::cout << "Loaded: " << loaded.width() << "x" << loaded.height()
                      << ", score=" << loaded.score() << ", pieces=";
            int count2 = 0;
            for (auto it = loaded.begin(); it != loaded.end(); ++it) count2++;
            std::cout << count2 << "\n";
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ Failed to load: " << e.what() << std::endl;
    }
}

void test_containment_simple() {
    std::cout << "\n=== TEST CONTAINMENT SIMPLE ===\n";

    tetris game(4, 4);

    // Pezzo 2x2 pieno
    piece p(2, 100);
    p(0,0) = p(0,1) = p(1,0) = p(1,1) = true;

    // Aggiungi un pezzo a (0,0)
    game.add(p, 0, 0);

    // Test vari containment
    std::cout << "Piece at (0,0), test containment at:\n";
    std::cout << "  (0,2): " << game.containment(p, 0, 2) << " (expected: 1, above)\n";
    std::cout << "  (0,0): " << game.containment(p, 0, 0) << " (expected: 0, collision)\n";
    std::cout << "  (2,0): " << game.containment(p, 2, 0) << " (expected: 1, right)\n";
    std::cout << "  (0,-1): " << game.containment(p, 0, -1) << " (expected: 0, y negative)\n";
    std::cout << "  (3,0): " << game.containment(p, 3, 0) << " (expected: 0, x out of bounds)\n";

    // Test con pezzo parzialmente sovrapposto
    std::cout << "\nTest partial overlap:\n";
    std::cout << "  (1,0): " << game.containment(p, 1, 0) << " (expected: 0, partial overlap)\n";
    std::cout << "  (0,1): " << game.containment(p, 0, 1) << " (expected: 0, partial overlap)\n";
}

void test_critical_insert_scenario() {
    std::cout << "\n=== TEST CRITICAL INSERT SCENARIO ===\n";

    // Simula un caso Tetris tipico
    tetris game(10, 6);  // Campo 10x6

    // Crea pezzo a L 3x3
    piece L(3, 100);
    // Forma a L:
    // X..
    // X..
    // XX.
    L(0,0) = true;  // TL
    L(1,0) = true;  // middle left
    L(2,0) = true;  // BL
    L(2,1) = true;  // BR-1

    // Crea pezzo a T 3x3
    piece T(3, 200);
    // Forma a T:
    // XXX
    // .X.
    // .X.
    T(0,0) = T(0,1) = T(0,2) = true;
    T(1,1) = true;
    T(2,1) = true;

    // Inserisci vari pezzi
    try {
        game.insert(L, 0);  // In basso a sinistra
        std::cout << "✓ Inserted L at x=0\n";

        game.insert(T, 3);  // Al centro
        std::cout << "✓ Inserted T at x=3\n";

        // Ruota e inserisci altro
        L.rotate();
        game.insert(L, 6);
        std::cout << "✓ Inserted rotated L at x=6\n";

        // Prova a riempire una riga
        piece block(1, 50);
        block(0,0) = true;

        // Riempi riga 0 (bottom)
        for (int x = 0; x < 10; x++) {
            // Controlla se la posizione è libera
            bool can_place = true;
            for (auto it = game.begin(); it != game.end() && can_place; ++it) {
                if (x >= it->x && x < it->x + it->p.side()) {
                    // C'è un pezzo qui
                    can_place = false;
                }
            }
            if (can_place) {
                game.insert(block, x);
            }
        }

        std::cout << "✓ Filled available spaces in bottom row\n";
        std::cout << "Final score: " << game.score() << "\n";
        std::cout << "Pieces count: ";
        int count = 0;
        for (auto it = game.begin(); it != game.end(); ++it) count++;
        std::cout << count << "\n";

        // Salva e ricarica
        std::stringstream ss;
        ss << game;
        std::cout << "\nGame saved (" << ss.str().length() << " bytes)\n";

        tetris game2;
        ss >> game2;

        if (game == game2) {
            std::cout << "✓ Round-trip successful!\n";
        } else {
            std::cout << "✗ Round-trip FAILED!\n";
        }

    } catch (const tetris_exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }
}
void test_proper_pieces() {
    std::cout << "\n=== TEST WITH PROPER SIDES ===\n";

    // Test side validi
    uint32_t valid_sides[] = {1, 2, 4, 8, 16};

    for (uint32_t side : valid_sides) {
        try {
            piece p(side, 100);
            std::cout << "✓ Created piece with side=" << side << "\n";
        } catch (const tetris_exception& e) {
            std::cout << "✗ Failed to create piece with side=" << side
                      << ": " << e.what() << "\n";
        }
    }

    // Test side non validi
    uint32_t invalid_sides[] = {0, 3, 5, 6, 7, 9, 10};

    for (uint32_t side : invalid_sides) {
        try {
            piece p(side, 100);
            std::cout << "✗ ERROR: Should have thrown for side=" << side << "\n";
        } catch (const tetris_exception& e) {
            std::cout << "✓ Correctly rejected side=" << side
                      << " (" << e.what() << ")\n";
        }
    }
}
void test_exact_play_game_format() {
    std::cout << "\n=== TEST EXACT PLAY GAME FORMAT ===\n";

    tetris game(8, 6);

    std::string input =
        "4 75 (([]()[]())(()[]()[])([]()()())(()[]()()))\n"
        "2 100 ()\n"
        "4 200 (()()()())\n";

    std::stringstream ss(input);

    int inserted_count = 0;
    while (ss.peek() != EOF) {
        piece p;
        try {
            if (!(ss >> p)) break;

            p.rotate();
            game.insert(p, 2);
            inserted_count++;
            std::cout << "✓ Inserted and rotated piece " << inserted_count << "\n";

        } catch (const tetris_exception& e) {
            std::cout << "✗ Error: " << e.what() << "\n";
            break;
        }
    }

    // Conta pezzi manualmente
    int piece_count = 0;
    for (auto it = game.begin(); it != game.end(); ++it) {
        piece_count++;
    }

    std::cout << "\nGame has " << piece_count << " pieces, score: " << game.score() << "\n";

    // Mostra tutti i pezzi
    std::cout << "Pieces positions: ";
    for (auto it = game.begin(); it != game.end(); ++it) {
        std::cout << "(" << it->x << "," << it->y << ") ";
    }
    std::cout << "\n";

    // Salva
    std::stringstream output;
    output << game;
    std::string saved = output.str();

    std::cout << "\nSaved game:\n" << saved << "---\n";

    // Ricarica
    output.seekg(0);
    tetris loaded;
    try {
        output >> loaded;
        std::cout << "✓ Loaded back\n";

        // Conta pezzi loaded
        int loaded_count = 0;
        for (auto it = loaded.begin(); it != loaded.end(); ++it) {
            loaded_count++;
        }

        std::cout << "Loaded game has " << loaded_count << " pieces, score: " << loaded.score() << "\n";

        // Confronta
        if (game == loaded) {
            std::cout << "✓ Games are EQUAL\n";
        } else {
            std::cout << "✗ Games are DIFFERENT\n";

            // Debug: mostra differenze
            if (game.score() != loaded.score()) {
                std::cout << "  Scores differ: " << game.score() << " vs " << loaded.score() << "\n";
            }
            if (piece_count != loaded_count) {
                std::cout << "  Piece counts differ: " << piece_count << " vs " << loaded_count << "\n";
            }
        }
    } catch (const tetris_exception& e) {
        std::cout << "✗ Load failed: " << e.what() << "\n";
    }
}

void test_insert_should_not_fail() {
    std::cout << "\n=== TEST INSERT SHOULD NOT FAIL ===\n";

    // Caso 1: Pezzi piccoli dovrebbero poter essere impilati
    {
        tetris game(8, 20);  // Campo alto
        piece small(2, 100);  // 2x2 pieno
        small(0,0) = small(0,1) = small(1,0) = small(1,1) = true;

        // Dovremmo poter inserire 10 pezzi (20/2 = 10)
        for (int i = 0; i < 10; i++) {
            try {
                game.insert(small, 0);
                std::cout << "Insert " << i << " at x=0: ✓ (y=";
                // Trova l'ultimo pezzo inserito
                auto it = game.begin();
                if (it != game.end()) {
                    std::cout << it->y;
                }
                std::cout << ")\n";
            } catch (const tetris_exception& e) {
                std::cout << "Insert " << i << " at x=0: ✗ " << e.what() << "\n";
                break;
            }
        }
    }

    // Caso 2: Pezzi a x diverse non dovrebbero collidere
    {
        tetris game(8, 6);
        piece p(2, 100);
        p(0,0) = p(0,1) = p(1,0) = p(1,1) = true;

        // Inserisci a x=0, x=2, x=4, x=6 (non dovrebbero collidere)
        for (int x = 0; x < 8; x += 2) {
            try {
                game.insert(p, x);
                std::cout << "Insert at x=" << x << ": ✓\n";
            } catch (const tetris_exception& e) {
                std::cout << "Insert at x=" << x << ": ✗ " << e.what() << "\n";
            }
        }
    }
}
#include <iostream>
#include <sstream>
#include <cassert>

void run_crazy_tests() {
    std::cout << "=== INIZIO TEST FOLLI ===" << std::endl;

    int test_count = 0;
    int passed = 0;

    // ==================== TEST 1: Piece con side=0 ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Piece side=0 (dovrebbe lanciare)" << std::endl;
        try {
            piece p(0, 1);  // side=0 NON valido
            std::cout << "  FAIL: Dovrebbe lanciare eccezione!" << std::endl;
        } catch (const tetris_exception&) {
            std::cout << "  PASS: Lancia correttamente" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "  FAIL: Lancia eccezione sbagliata" << std::endl;
        }
    }

    // ==================== TEST 2: Piece con side non potenza di 2 ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Piece side=3 (non potenza di 2)" << std::endl;
        try {
            piece p(3, 1);  // 3 non è potenza di 2
            std::cout << "  FAIL: Dovrebbe lanciare!" << std::endl;
        } catch (const tetris_exception&) {
            std::cout << "  PASS: Lancia correttamente" << std::endl;
            passed++;
        }
    }

    // ==================== TEST 3: Piece color=0 (non valido) ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Piece color=0" << std::endl;
        try {
            piece p(2, 0);  // color=0 non valido
            std::cout << "  FAIL: Dovrebbe lanciare!" << std::endl;
        } catch (const tetris_exception&) {
            std::cout << "  PASS: Lancia correttamente" << std::endl;
            passed++;
        }
    }

    // ==================== TEST 4: Piece color=255 (massimo) ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Piece color=255 (massimo)" << std::endl;
        try {
            piece p(2, 255);  // color=255 valido
            std::cout << "  PASS: Accetta color=255" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "  FAIL: Non dovrebbe lanciare" << std::endl;
        }
    }

    // ==================== TEST 5: Piece 1x1 (side minimo) ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Piece 1x1" << std::endl;
        try {
            piece p(1, 1);  // side=1 (potenza di 2)
            p(0,0) = true;
            assert(p.full());
            std::cout << "  PASS: Piece 1x1 funziona" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "  FAIL: Piece 1x1 non funziona" << std::endl;
        }
    }

    // ==================== TEST 6: Tetris dimensioni 0 ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Tetris 0x0" << std::endl;
        try {
            tetris t(0, 10, 0);  // width=0
            std::cout << "  FAIL: Dovrebbe lanciare!" << std::endl;
        } catch (const tetris_exception&) {
            std::cout << "  PASS: Lancia correttamente" << std::endl;
            passed++;
        }
    }

    // ==================== TEST 7: Inserimento pezzo vuoto ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Inserisci pezzo vuoto" << std::endl;
        try {
            tetris t(10, 20, 0);
            piece p(4, 1);  // Tutte false
            t.insert(p, 0);  // Pezzo vuoto dovrebbe essere ignorato
            std::cout << "  PASS: Pezzo vuoto ignorato" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "  FAIL: Non dovrebbe lanciare" << std::endl;
        }
    }

    // ==================== TEST 8: Inserimento con x negativo estremo ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Insert con x=-100" << std::endl;
        try {
            tetris t(10, 20, 0);
            piece p(2, 1);
            p(0,0) = true;  // Solo una cella
            p(1,1) = true;

            // x=-100 ma le celle occupate sono a j=0 e j=1
            // Quindi field_x = -100+0 = -100 e -100+1 = -99 -> FUORI!
            t.insert(p, -100);
            std::cout << "  FAIL: Dovrebbe lanciare GAME OVER!" << std::endl;
        } catch (const tetris_exception& e) {
            if (std::string(e.what()) == "GAME OVER") {
                std::cout << "  PASS: GAME OVER corretto" << std::endl;
                passed++;
            } else {
                std::cout << "  FAIL: Eccezione sbagliata: " << e.what() << std::endl;
            }
        } catch (...) {
            std::cout << "  FAIL: Eccezione sconosciuta" << std::endl;
        }
    }

    // ==================== TEST 9: Inserimento pezzo più largo del campo ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Piece 32x32 in campo 10x20" << std::endl;
        try {
            tetris t(10, 20, 0);
            piece p(32, 1);

            // Riempi solo la parte bassa del pezzo
            for (uint32_t i = 20; i < 32; i++) {  // Solo righe 20-31 (quelle che stanno nel campo)
                for (uint32_t j = 0; j < 32; j++) {
                    if (j < 10) {  // Solo colonne 0-9 (dentro campo)
                        p(i, j) = true;
                    }
                }
            }

            t.insert(p, 0);
            std::cout << "  PASS: Pezzo parzialmente nel campo" << std::endl;
            passed++;
        } catch (const tetris_exception& e) {
            std::cout << "  Result: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "  FAIL: Eccezione sconosciuta" << std::endl;
        }
    }

    // ==================== TEST 10: Parser input malformato ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Parser '4 1 ([' (malformato)" << std::endl;
        try {
            std::stringstream ss("4 1 ([");
            piece p;
            ss >> p;
            std::cout << "  FAIL: Dovrebbe lanciare!" << std::endl;
        } catch (const tetris_exception&) {
            std::cout << "  PASS: Lancia correttamente" << std::endl;
            passed++;
        }
    }

    // ==================== TEST 11: Cut row su pezzo vuoto ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": cut_row su pezzo vuoto" << std::endl;
        try {
            piece p(4, 1);  // Tutto false
            p.cut_row(0);   // Dovrebbe funzionare
            assert(p.empty());
            std::cout << "  PASS: cut_row su vuoto ok" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "  FAIL: Non dovrebbe lanciare" << std::endl;
        }
    }

    // ==================== TEST 12: cut_row con indice fuori range ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": cut_row(10) su pezzo 4x4" << std::endl;
        try {
            piece p(4, 1);
            p.cut_row(10);  // 10 >= 4
            std::cout << "  FAIL: Dovrebbe lanciare!" << std::endl;
        } catch (const tetris_exception&) {
            std::cout << "  PASS: Lancia correttamente" << std::endl;
            passed++;
        }
    }

    // ==================== TEST 13: Operatore() con indici fuori range ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": p(10,0) su pezzo 4x4" << std::endl;
        try {
            piece p(4, 1);
            p(10, 0) = true;  // i=10 >= 4
            std::cout << "  FAIL: Dovrebbe lanciare!" << std::endl;
        } catch (const tetris_exception&) {
            std::cout << "  PASS: Lancia correttamente" << std::endl;
            passed++;
        }
    }

    // ==================== TEST 14: Molte rotazioni ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": 1000 rotazioni" << std::endl;
        try {
            piece p(4, 1);
            // Crea forma a L
            p(0,0)=p(1,0)=p(2,0)=p(2,1)=true;

            piece original = p;

            for (int i = 0; i < 1000; i++) {
                p.rotate();
            }

            // Dopo 4 rotazioni torna uguale, dopo 1000 anche
            // 1000 % 4 = 0, quindi dovrebbe essere uguale all'originale
            assert(p == original);
            std::cout << "  PASS: 1000 rotazioni ok" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "  FAIL: Crash nelle rotazioni" << std::endl;
        }
    }

    // ==================== TEST 15: Memory leak test ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Molti pezzi e tetris" << std::endl;
        try {
            // Crea e distruggi molti oggetti
            for (int k = 0; k < 100; k++) {
                tetris t(10, 20, 0);
                for (int i = 0; i < 50; i++) {
                    piece p(4, i % 255 + 1);
                    // Forma diversa ogni volta
                    p(i % 4, i % 4) = true;
                    t.insert(p, i % 8);
                }

                // Copia
                tetris t2 = t;
                // Move
                tetris t3 = std::move(t2);

                // Assegnazione
                tetris t4(5, 5, 0);
                t4 = t3;
                t4 = std::move(t3);
            }
            std::cout << "  PASS: No crash (verifica con valgrind per leak)" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "  FAIL: Crash in stress test" << std::endl;
        }
    }

    // ==================== TEST 16: Piece enorme ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Piece 1024x1024" << std::endl;
        try {
            piece p(1024, 1);  // Grande allocazione
            p(0,0) = true;
            p(1023, 1023) = true;
            p.rotate();
            std::cout << "  PASS: Piece enorme ok (se memoria sufficiente)" << std::endl;
            passed++;
        } catch (const std::bad_alloc&) {
            std::cout << "  SKIP: Memoria insufficiente" << std::endl;
            passed++;  // Non è un fallimento
        } catch (...) {
            std::cout << "  FAIL: Crash con piece grande" << std::endl;
        }
    }

    // ==================== TEST 17: Stream operator con spazi strani ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": Parser con molti spazi" << std::endl;
        try {
            std::stringstream ss("   4   75   ( ( [ ] ( ) [ ] ( ) ) ( ( ) [ ] ( ) [ ] ) ( [ ] ( ) ( ) ( ) ) ( ( ) [ ] ( ) ( ) ) )   ");
            piece p;
            ss >> p;
            std::cout << "  PASS: Parser gestisce spazi" << std::endl;

            // Verifica scrittura
            std::stringstream ss2;
            ss2 << p;
            piece p2;
            ss2 >> p2;
            assert(p == p2);
            std::cout << "  PASS: Round-trip ok" << std::endl;
            passed++;
        } catch (const tetris_exception& e) {
            std::cout << "  FAIL: " << e.what() << std::endl;
        }
    }

    // ==================== TEST 18: Contenimento impossibile ====================
    {
        test_count++;
        std::cout << "Test " << test_count << ": containment impossibile" << std::endl;
        try {
            tetris t(5, 5, 0);

            // Primo pezzo
            piece p1(4, 1);
            p1(3,3)=true;  // Solo bottom-right
            t.insert(p1, 0);  // y sarà 4 (bottom a fondo)
            const tetris_piece& tp = *(t.begin());
            std::cout << "Primo pezzo: x=" << tp.x << ", y=" << tp.y << std::endl;
            std::cout << "Cella occupata nel pezzo: (3,3)" << std::endl;
            std::cout << "Cella nel campo: (" << (tp.x + 3) << "," << (tp.y - 3) << ")" << std::endl;
            // Secondo pezzo nello stesso posto
            std::cout << "Secondo pezzo a x=3, y=4" << std::endl;
            std::cout << "Cella occupata nel pezzo: (0,0)" << std::endl;
            std::cout << "Cella nel campo: (" << (3 + 0) << "," << (4 - 0) << ")" << std::endl;
            piece p2(2, 2);
            p2(0,0)=true;

            // Dovrebbe fallire containment
            bool can = t.containment(p2, 3, 4);  // Stessa cella occupata
            if (!can) {
                std::cout << "  PASS: containment rileva collisione" << std::endl;
                passed++;
            } else {
                std::cout << "  FAIL: containment non rileva collisione" << std::endl;
            }
        } catch (...) {
            std::cout << "  FAIL: Crash in containment test" << std::endl;
        }
    }

    // ==================== RISULTATI ====================
    std::cout << "\n=== RISULTATI ===" << std::endl;
    std::cout << "Test eseguiti: " << test_count << std::endl;
    std::cout << "Test passati: " << passed << std::endl;
    std::cout << "Test falliti: " << (test_count - passed) << std::endl;

    if (passed == test_count) {
        std::cout << "SUCCESSO: Tutti i test passati!" << std::endl;
    } else {
        std::cout << "ATTENZIONE: Alcuni test falliti" << std::endl;
    }
}

void test_cut_row_zero() {
    std::cout << "=== TEST cut_row(0) ===" << std::endl;

    // Test 1: Piece 4x4 pieno, cut_row(0) dovrebbe eliminare la riga top
    {
        std::cout << "Test 1: Piece 4x4 pieno, cut_row(0)" << std::endl;
        piece p(4, 1);

        // Riempi tutto
        for (uint32_t i = 0; i < 4; ++i) {
            for (uint32_t j = 0; j < 4; ++j) {
                p(i, j) = true;
            }
        }

        std::cout << "Prima di cut_row(0):" << std::endl;
        std::cout << "  full(): " << p.full() << " (dovrebbe essere 1)" << std::endl;

        p.cut_row(0);  // Taglia la riga top (riga 0)

        std::cout << "Dopo cut_row(0):" << std::endl;
        std::cout << "  full(): " << p.full() << " (dovrebbe essere 0)" << std::endl;

        // Verifica che la riga 0 sia ora vuota
        bool row0_empty = true;
        for (uint32_t j = 0; j < 4; ++j) {
            if (p(0, j)) row0_empty = false;
        }
        std::cout << "  riga 0 vuota: " << row0_empty << " (dovrebbe essere 1)" << std::endl;

        // Verifica che le altre righe siano piene (shiftate in alto)
        bool other_rows_full = true;
        for (uint32_t i = 1; i < 4; ++i) {
            for (uint32_t j = 0; j < 4; ++j) {
                if (!p(i, j)) other_rows_full = false;
            }
        }
        std::cout << "  righe 1-3 piene: " << other_rows_full << " (dovrebbe essere 1)" << std::endl;

        if (row0_empty && other_rows_full) {
            std::cout << "  PASS" << std::endl;
        } else {
            std::cout << "  FAIL" << std::endl;
        }
    }

    std::cout << std::endl;

    // Test 2: Piece 4x4 con pattern, cut_row(0)
    {
        std::cout << "Test 2: Piece 4x4 con pattern specifico" << std::endl;
        piece p(4, 2);

        // Crea pattern:
        // Riga 0: X X X X
        // Riga 1: X . X .
        // Riga 2: . X . X
        // Riga 3: X X . .
        p(0,0)=p(0,1)=p(0,2)=p(0,3)=true;  // Riga 0 piena
        p(1,0)=true; p(1,1)=false; p(1,2)=true; p(1,3)=false;
        p(2,0)=false; p(2,1)=true; p(2,2)=false; p(2,3)=true;
        p(3,0)=true; p(3,1)=true; p(3,2)=false; p(3,3)=false;

        std::cout << "Pattern iniziale:" << std::endl;
        for (uint32_t i = 0; i < 4; ++i) {
            std::cout << "  Riga " << i << ": ";
            for (uint32_t j = 0; j < 4; ++j) {
                std::cout << (p(i, j) ? 'X' : '.') << ' ';
            }
            std::cout << std::endl;
        }

        p.cut_row(0);

        std::cout << "Dopo cut_row(0):" << std::endl;
        for (uint32_t i = 0; i < 4; ++i) {
            std::cout << "  Riga " << i << ": ";
            for (uint32_t j = 0; j < 4; ++j) {
                std::cout << (p(i, j) ? 'X' : '.') << ' ';
            }
            std::cout << std::endl;
        }

        // Dovrebbe essere:
        // Riga 0: X . X .  (ex riga 1)
        // Riga 1: . X . X  (ex riga 2)
        // Riga 2: X X . .  (ex riga 3)
        // Riga 3: . . . .  (nuova riga vuota)

        bool correct = true;
        // Riga 0
        if (!p(0,0) || p(0,1) || !p(0,2) || p(0,3)) correct = false;
        // Riga 1
        if (p(1,0) || !p(1,1) || p(1,2) || !p(1,3)) correct = false;
        // Riga 2
        if (!p(2,0) || !p(2,1) || p(2,2) || p(2,3)) correct = false;
        // Riga 3 (deve essere tutta falsa)
        for (uint32_t j = 0; j < 4; ++j) {
            if (p(3, j)) correct = false;
        }

        if (correct) {
            std::cout << "  PASS" << std::endl;
        } else {
            std::cout << "  FAIL" << std::endl;
        }
    }

    std::cout << std::endl;

    // Test 3: Piece 1x1, cut_row(0) - caso limite
    {
        std::cout << "Test 3: Piece 1x1, cut_row(0)" << std::endl;
        piece p(1, 3);
        p(0,0) = true;

        std::cout << "Prima: p(0,0) = " << p(0,0) << std::endl;
        p.cut_row(0);
        std::cout << "Dopo: p(0,0) = " << p(0,0) << std::endl;

        if (!p(0,0)) {
            std::cout << "  PASS: Riga diventa vuota" << std::endl;
        } else {
            std::cout << "  FAIL: Riga dovrebbe essere vuota" << std::endl;
        }
    }

    std::cout << std::endl;

    // Test 4: Piece vuoto, cut_row(0)
    {
        std::cout << "Test 4: Piece vuoto 4x4, cut_row(0)" << std::endl;
        piece p(4, 4);  // Tutto false per default

        try {
            p.cut_row(0);
            std::cout << "  Dopo cut_row(0) su pezzo vuoto" << std::endl;

            // Verifica che sia ancora vuoto
            if (p.empty()) {
                std::cout << "  PASS: Rimane vuoto" << std::endl;
            } else {
                std::cout << "  FAIL: Non dovrebbe avere celle true" << std::endl;
            }
        } catch (...) {
            std::cout << "  FAIL: Eccezione inattesa" << std::endl;
        }
    }

    std::cout << std::endl;

    // Test 5: cut_row(0) multipli
    {
        std::cout << "Test 5: 3 cut_row(0) consecutivi su pezzo 3x3" << std::endl;
        piece p(4, 5);

        // Riempi
        for (uint32_t i = 0; i < 4; ++i) {
            for (uint32_t j = 0; j < 4; ++j) {
                p(i, j) = true;
            }
        }

        std::cout << "Prima: full() = " << p.full() << std::endl;

        p.cut_row(0);
        std::cout << "Dopo 1 cut: riga 0 vuota? " << !p(0,0) << std::endl;

        p.cut_row(0);  // Ancora sulla nuova riga 0 (ex riga 1)
        std::cout << "Dopo 2 cut: " << std::endl;

        p.cut_row(0);
        std::cout << "Dopo 3 cut: empty() = " << p.empty() << std::endl;

        if (p.empty()) {
            std::cout << "  PASS: Dopo 3 cut diventa vuoto" << std::endl;
        } else {
            std::cout << "  FAIL: Dovrebbe essere vuoto" << std::endl;
        }
    }

    std::cout << std::endl;

    // Test 6: cut_row(0) in tetris context
    {
        std::cout << "Test 6: cut_row(0) nel contesto tetris" << std::endl;
        tetris t(4, 4, 0);

        piece p(2, 6);
        p(0,0)=p(0,1)=p(1,0)=p(1,1)=true;  // Pezzo 2x2 pieno

        t.insert(p, 0);  // Dovrebbe andare a y=3? (bottom a fondo)

        std::cout << "Prima del taglio:" << std::endl;
        t.print_ascii_art(std::cout);

        // Simula taglio riga (manualmente per test)
        std::cout << "Dopo cut_row(0) sul pezzo:" << std::endl;
        p.cut_row(0);

        // Mostra pezzo modificato
        std::cout << "Pezzo dopo cut_row(0):" << std::endl;
        for (uint32_t i = 0; i < 2; ++i) {
            std::cout << "  ";
            for (uint32_t j = 0; j < 2; ++j) {
                std::cout << (p(i, j) ? 'X' : '.');
            }
            std::cout << std::endl;
        }

        std::cout << "  Test completato" << std::endl;
    }

    std::cout << "=== FINE TEST cut_row(0) ===" << std::endl;
}


int main() {
    try {
        std::cout << "🎮 TEST COMPLETO TETRIS - VERSIONE FINALE 🎮" << std::endl;
        std::cout << "============================================" << std::endl;
        test_cut_row_zero();
        run_crazy_tests();
        /*
        test_tetris_io();
        test_piece_empty_full();
        simulate_play_game_test();
        test_with_whitespace();
        test_rotate_and_insert();
        test_complete_roundtrip();
        test_containment_simple();
        test_proper_pieces();
        test_exact_play_game_format();
        test_insert_should_not_fail();
        test_critical_insert_scenario();
        */
        //test_specific_failure();
        //test_step_by_step();
        //test_2x2_full();
        //test_read_complete_game();
        //test_parsing_and_rotation();
        //test_piece_roundtrip();
        /*
        test_tetris_game_session();

        // TEST PIECE (già verificati)
        debug_insert_game_over();
        test_with_spec_example();
        test_insert_no_negative_y();
        test_height_20_vs_6();
        test_cut_row_edge_case();
        test_io_edge_cases();
        test_whitespace_parsing();
        debug_add_failure();
        debug_parser_expansion();
        // Test manuale
        // Verifica l'ordine: dovrebbe essere C -> B -> A
        // perché add() inserisce in testa, ma abbiamo invertito l'ordine di lettura
        debug_cut_row_invalid_piece();
        test_cut_row_spec_example();
        test_height_1();
        final_integration_test();
        test_abbreviated_formats();
        test_explicit_formats();

        test_cut_row_thorough();
        test_official_cases();
        test_critical_cases();

        debug_insert_negative_x();
        test_example_from_spec();
        test_partial_piece_negative_x();
        test_negative_coordinates();
        debug_insert_complex_pieces();
        debug_insert_with_rotation();

        debug_tetris_read_specific();
        debug_containment_edge_cases();
        debug_insert_complex_cases();
        final_comprehensive_test();

        test_cut_row_specific();
        test_parser_errors();
        test_cut_row_manual();
        test_piece_rotate();
        test_piece_constructor();
        test_piece_empty_full();

        test_piece_cut_row();
        test_piece_copy_move();
        test_piece_streams();

        // TEST TETRIS
        test_tetris_constructor();
        test_tetris_containment();

        // ⚠️ Usa la versione CORRETTA di test_tetris_add_insert
        test_tetris_add_insert(); // Versione corretta sopra

        test_tetris_copy_move();
        test_tetris_iterators();
        test_tetris_streams();
        test_edge_cases();
        test_specific_failures();

        std::cout << "\n🎉 🎉 🎉 TUTTI I TEST SUPERATI! PROGETTO PRONTO! 🎉 🎉 🎉" << std::endl;
        std::cout << "Tutte le funzionalità di Piece e Tetris funzionano correttamente." << std::endl;
        std::cout << "Zero memory leak rilevati da Valgrind." << std::endl;
    */
    } catch (const std::exception& e) {
        std::cerr << "❌ ERRORE: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}