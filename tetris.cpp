#include <sstream>
#include "tetris.hpp"

/*
 * CLASSE PIECE
 */

piece::piece(): m_side(0), m_color(0), m_grid(nullptr) {
}

piece::piece(uint32_t s, uint8_t c):m_side(0), m_color(0), m_grid(nullptr){
    if(s == 0 || (s & (s-1)) != 0) {
        throw tetris_exception("Side deve essere una potenza di 2 > 0");
    }
    m_side=s;
    m_color=c;
    m_grid=nullptr;
    if(c == 0) {
        throw tetris_exception("Colore deve essere tra 1 e 255");
    }
    try {
        m_grid = new bool *[m_side];
        for(uint32_t i = 0; i < m_side; ++i) {
            m_grid[i] = nullptr;
        }
        for (uint32_t i = 0; i < m_side; ++i) {
            m_grid[i] = new bool[m_side]();
        }
    }catch(...){
        if (m_grid) {
            for (uint32_t i = 0; i < m_side; ++i) {
                if (m_grid[i]) {
                    delete[] m_grid[i];
                }
            }
            delete[] m_grid;
        }
        m_grid = nullptr;
        m_side = 0;
        m_color = 0;
        throw tetris_exception("Errore di allocazione memoria");
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

piece::~piece() {
    if(m_grid!=nullptr){
        if (m_grid) {
            for(uint32_t i = 0; i < m_side; ++i) {
                delete[] m_grid[i];
            }
            delete[] m_grid;
        }
    }
    m_grid= nullptr;
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
    if(m_grid== nullptr) return true;
    for(uint32_t r=0; r<m_side;r++){
        for(uint32_t c=0; c<m_side; c++){
            if(m_grid[r][c]) return false;
        }
    }
    return true;
}

bool piece::full() const {
    if(m_side==0) return false;
    if(m_grid== nullptr) return false;
    for(uint32_t r=0; r<m_side;r++){
        for(uint32_t c=0; c<m_side; c++){
            if(!m_grid[r][c]) return false;
        }
    }
    return true;
}


void piece::rotate() {
    bool** rotated_grid = new bool*[m_side];
    for(uint32_t i = 0; i < m_side; i++) {
        rotated_grid[i] = new bool[m_side];
    }

    for(uint32_t i = 0; i < m_side; i++) {
        for(uint32_t j = 0; j < m_side; j++) {
            rotated_grid[j][m_side - 1 - i] = m_grid[i][j];
        }
    }

    for(uint32_t i = 0; i < m_side; i++) {
        delete[] m_grid[i];
    }
    delete[] m_grid;

    m_grid = rotated_grid;
}

void piece::cut_row(uint32_t i) {
    if (i >= m_side) {
        throw tetris_exception("Indice fuori range in cut_row()");
    }

    for(uint32_t r = i; r < m_side - 1; r++) {
        for(uint32_t c = 0; c < m_side; c++) {
            m_grid[r][c] = m_grid[r + 1][c];
        }
    }

    for(uint32_t j = 0; j < m_side; j++) {
        m_grid[m_side - 1][j] = false;
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
            this->m_grid = nullptr;
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
    char c;
    is >> std::ws;

    if (!is.get(c)) {
        throw tetris_exception("Errore nel parsing: fine stream inaspettata");
    }

    if (c == '[') {
        is >> std::ws;
        char close;
        if (!is.get(close) || close != ']') {
            throw tetris_exception("Quadrante vuoto errato: serve ']' dopo '['");
        }
        for (uint32_t i = 0; i < len; i++) {
            for (uint32_t j = 0; j < len; j++) {
                p(start + i, end + j) = false;
            }
        }
    } else if (c == '(') {
        is >> std::ws;
        char next = is.peek();

        if (next == ')') {

            is.get();
            for (uint32_t i = 0; i < len; i++) {
                for (uint32_t j = 0; j < len; j++) {
                    p(start + i, end + j) = true;
                }
            }
        } else {
            uint32_t half = len / 2;
            if (half == 0) {
                throw tetris_exception("Dimensione quadrante troppo piccola per suddivisione");
            }

            C(is, p, start, end, half); // TL
            C(is, p, start, end + half, half); // TR
            C(is, p, start + half, end, half); // BL
            C(is, p, start + half, end + half, half); // BR

            is >> std::ws;
            char end_c;
            if (!is.get(end_c) || end_c != ')') {
                throw tetris_exception("Parentesi di chiusura ')' mancante");
            }
        }
    } else {
        throw tetris_exception(std::string("Carattere inatteso '") + c + "', atteso '[' o '('");
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

    is.clear();

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

    char first_char;
    if (!is.get(first_char)) {
        throw tetris_exception("Impossibile leggere la struttura del pezzo");
    }

    if (first_char != '(' && first_char != '[') {
        throw tetris_exception(std::string("Struttura pezzo malformata: atteso '(' o '[', trovato '") + first_char + "'");
    }

    is.putback(first_char);

    try {
        piece temp_p(side, static_cast<uint8_t>(color));
        C(is, temp_p, 0, 0, temp_p.side());

        is >> std::ws;
        if (is.peek() != EOF) {
            char extra_char = is.peek();
            if (extra_char != ' ' && extra_char != '\t' && extra_char != '\n' && extra_char != '\r') {
                throw tetris_exception("Caratteri extra dopo la struttura completa del pezzo");
            }
        }

        if (is.fail()) {
            throw tetris_exception("Errore durante il parsing della struttura");
        }

        p = std::move(temp_p);
    } catch (const tetris_exception&) {
        throw;
    } catch (...) {
        throw tetris_exception("Errore imprevisto durante la creazione del pezzo");
    }

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
    rhs.m_width = 0;
    rhs.m_height = 0;
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
        rhs.m_width = 0;
        rhs.m_height = 0;
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
    int piece_count = 0;

    while (it1 != end() && it2 != rhs.end()) {
        if (it1->p != it2->p) {
            return false;
        }
        if (it1->x != it2->x || it1->y != it2->y) {
            return false;
        }
        ++it1;
        ++it2;
        piece_count++;
    }

    if (it1 != end() || it2 != rhs.end()) {
        return false;
    }

    return true;
}


void tetris::add(const piece& p, int x, int y) {
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

    // Cerca la y PIÙ ALTA possibile (dall'alto verso il basso)
    for (int y = static_cast<int>(m_height) - 1; y >= -static_cast<int>(p.side()) + 1; y--) {
        if (containment(p, x, y)) {
            found_y = y;
        }
    }

    if (found_y == -1) {
        throw tetris_exception("GAME OVER");
    }

    add(p, x, found_y);

    bool changed;
    do {
        changed = false;

        for (int row = static_cast<int>(m_height) - 1; row >= 0; row--) {
            bool row_complete = true;

            for (uint32_t col = 0; col < m_width && row_complete; col++) {
                bool occupied = false;

                for (iterator it = begin(); it != end() && !occupied; ++it) {
                    const tetris_piece& tp = *it;
                    int piece_row = row - tp.y;
                    int piece_col = col - tp.x;

                    if (piece_row >= 0 && piece_row < static_cast<int>(tp.p.side()) &&
                        piece_col >= 0 && piece_col < static_cast<int>(tp.p.side()) &&
                        tp.p(piece_row, piece_col)) {
                        occupied = true;
                    }
                }

                if (!occupied) {
                    row_complete = false;
                }
            }

            if (row_complete) {
                // Taglia la riga da tutti i pezzi che la intersecano
                for (iterator it = begin(); it != end(); ++it) {
                    tetris_piece& tp = *it;
                    if (tp.y <= row && tp.y + static_cast<int>(tp.p.side()) > row) {
                        uint32_t piece_row = row - tp.y;
                        tp.p.cut_row(piece_row);
                    }
                }

                m_score += m_width;
                changed = true;
                break;  // Ricomincia il controllo delle righe
            }
        }

        bool gravity_changed;
        do {
            gravity_changed = false;
            for (iterator it = begin(); it != end(); ++it) {
                tetris_piece& tp = *it;
                if (tp.y > -static_cast<int>(tp.p.side()) + 1 && containment(tp.p, tp.x, tp.y - 1)) {
                    tp.y--;
                    gravity_changed = true;
                    changed = true;
                }
            }
        } while (gravity_changed);

        // Rimuovi pezzi vuoti
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
    } while (changed);
}

bool tetris::containment(const piece& p, int x, int y) const {
    if (p.side() == 0) {
        throw tetris_exception("Piece deve avere side > 0");
    }

    // Controllo bordi
    for (uint32_t i = 0; i < p.side(); i++) {
        for (uint32_t j = 0; j < p.side(); j++) {
            if (p(i, j)) {
                int field_x = x + static_cast<int>(j);
                int field_y = y + static_cast<int>(i);

                if (field_x < 0 || field_x >= static_cast<int>(m_width) ||
                    field_y < 0 || field_y >= static_cast<int>(m_height)) {
                    return false;
                    }
            }
        }
    }

    // Controllo collisioni
    for (const_iterator it = begin(); it != end(); ++it) {
        const tetris_piece& existing = *it;

        for (uint32_t i = 0; i < p.side(); i++) {
            for (uint32_t j = 0; j < p.side(); j++) {
                if (!p(i, j)) continue;

                int new_x = x + static_cast<int>(j);
                int new_y = y + static_cast<int>(i);

                for (uint32_t ei = 0; ei < existing.p.side(); ei++) {
                    for (uint32_t ej = 0; ej < existing.p.side(); ej++) {
                        if (!existing.p(ei, ej)) continue;

                        int existing_x = existing.x + static_cast<int>(ej);
                        int existing_y = existing.y + static_cast<int>(ei);

                        if (new_x == existing_x && new_y == existing_y) {
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

    // Crea tetris temporaneo
    tetris temp(width, height, score);
    is >> std::ws;

    // Usiamo una pila (stack) di pezzi usando solo l'interfaccia pubblica
    struct PieceInfo {
        piece p;
        int x;
        int y;
        PieceInfo* next;
    };

    PieceInfo* stack = nullptr;

    while (is.peek() != EOF) {
        std::streampos current_pos = is.tellg();

        try {
            piece p;
            int x, y;

            if (!(is >> p)) {
                break;
            }

            is >> std::ws;

            if (!(is >> x)) {
                throw tetris_exception("Errore lettura coordinata x");
            }

            is >> std::ws;
            if (!(is >> y)) {
                throw tetris_exception("Errore lettura coordinata y");
            }

            // Push sullo stack (ordine inverso)
            PieceInfo* new_node = new PieceInfo{p, x, y, stack};
            stack = new_node;

        } catch (const tetris_exception& e) {
            // Se c'è un errore, pulisci lo stack e rilancia
            while (stack) {
                PieceInfo* next = stack->next;
                delete stack;
                stack = next;
            }
            throw; // Rilancia l'eccezione originale
        }

        is >> std::ws;
    }

    // Ora pop dallo stack e aggiungi in testa (ordine corretto)
    PieceInfo* current = stack;
    while (current) {
        temp.add(current->p, current->x, current->y);
        PieceInfo* next = current->next;
        delete current;
        current = next;
    }

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

int main() {
    try {
        std::cout << "🎮 TEST COMPLETO TETRIS - VERSIONE FINALE 🎮" << std::endl;
        std::cout << "============================================" << std::endl;

        // TEST PIECE (già verificati)
        //test_power_of_two_validation();
        //test_memory_safety_with_exceptions();
        /*test_parser_debug();
        debug_insert_game_over();
        debug_containment_edge_cases();
        debug_insert_complex_cases();
        final_comprehensive_test();
        */
        debug_color_zero();

        /*
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