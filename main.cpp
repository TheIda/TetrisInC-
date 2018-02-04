#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <memory>
#include <chrono>

#include "rlutil.h"

int selectLevel;

using Matrix = std::vector<std::vector<int>>;

struct Point2D{
    int x, y;
};

namespace{ //declaring identifiers
    
    enum playWidth{
        EMPTY = 0,
        WALL  = 9
    };
    
    enum Menu{
        PLAY = 1,
        EXIT = 0
    };
    
    enum nblock{
        FROM = 0,
        TO   = 4,
    };
    
    enum bHeight{
        CE_FROM  = 0,
        CE_TO    = 21
    };
    
    enum nRow{
        RE_FROM  = 0,
        RE_TO    = 12
    };
    
    template<typename T, bool enable = std::is_enum<T>::value>
    T& operator++(T& e){
        return e = static_cast<T>(e + 1);
    }
};

template<typename T, bool enable = std::is_integral<T>::value || std::is_enum<T>::value>
struct range_impl{
    struct iterator{
        const T operator * () const noexcept{
            return value;
        }
        
        iterator& operator ++() noexcept{
            ++value;
            return *this;
        }
        
        friend  const bool operator != (const iterator& lhs, const iterator& rhs) noexcept{
            return lhs.value != rhs.value;
        }
        
        T value;
    };
    
    std::size_t size() const{
        return last - first;
    }
    
    const iterator begin() const noexcept{
        return{ first };
    }
    
    const iterator end() const noexcept{
        return{
            last
        };
    }
    
    T first;
    T last;
};

template<typename T>
struct range_impl<T, false>{
    range_impl(T first, T last)
    : first(first)
    , last(last){
    }
    
    std::size_t size() const{
        return std::distance(first, last);
    }
    
    const T begin() const noexcept{
        return{ first
        };
    }
    
    const T end() const noexcept{
        return{ last
        };
    }
    
    T first;
    T last;
};

template<typename T1, typename T2>
range_impl<typename std::common_type<T1, T2>::type>
range(T1 first, T2 last) noexcept{
    return{
        first, last
    };
}

class Shape{
public:
    Shape() = default;
    
    virtual ~Shape() = default;
    virtual Shape *clone() const = 0;
    virtual int nCoordinate(std::size_t i, std::size_t j) const = 0;
    virtual Matrix rotate() = 0;
    virtual Matrix& shape() = 0;
    
    std::size_t size() const{
        return range(nblock::FROM, nblock::TO).size();
    }
};


template <typename Derived> //gives all shapes the same function
struct Interfaceable : public Shape{
    virtual Shape *clone() const override{
        return new Derived(static_cast<const Derived&>(*this));
    }
    
    virtual int nCoordinate(std::size_t i, std::size_t j) const override{
        return static_cast<const Derived&>(*this).shape[i][j];
    }
    
    virtual Matrix rotate() override{ //rotate
        for (const auto i : range(nblock::FROM, nblock::TO)){
            for (const auto& j : range(nblock::FROM, nblock::TO)){
                if (i < j){
                    std::swap(static_cast<Derived&>(*this).shape[i][j], static_cast<Derived&>(*this).shape[j][i]);
                }
            }
            
            std::reverse(static_cast< Derived&>(*this).shape[i].begin(), static_cast<Derived&>(*this).shape[i].end());
        }
        
        return static_cast<Derived&>(*this).shape;
    }
    
    virtual Matrix& shape() override{
        return static_cast<Derived&>(*this).shape;
    }
};

namespace shapes{
    class O : public Interfaceable<O>{
    public:
        O() = default;
        virtual ~O() = default;
        
        Matrix shape{
            {
                { 0, 0, 0, 0 },
                { 0, 1, 1, 0 },
                { 0, 1, 1, 0 },
                { 0, 0, 0, 0 }
            }
        };
    };
    
    class L : public Interfaceable<L>{
    public:
        L() = default;
        virtual ~L() = default;
        
        Matrix shape{
            {
                { 0, 0, 0, 0 },
                { 0, 1, 1, 0 },
                { 0, 0, 1, 0 },
                { 0, 0, 1, 0 }
            }
        };
    };
    
    class M : public Interfaceable<M>{
    public:
        M() = default;
        virtual ~M() = default;
        
        Matrix shape{
            {
                { 0, 1, 0, 0 },
                { 0, 1, 1, 0 },
                { 0, 0, 1, 0 },
                { 0, 0, 0, 0 }
            }
        };
    };
    
    class N : public Interfaceable<N>{
    public:
        N() = default;
        virtual ~N() = default;
        
        Matrix shape{
            {
                { 0, 0, 1, 0 },
                { 0, 1, 1, 0 },
                { 0, 1, 0, 0 },
                { 0, 0, 0, 0 }
            }
        };
    };
    
    class T : public Interfaceable<T>{
    public:
        T() = default;
        virtual ~T() = default;
        
        Matrix shape{
            {
                { 0, 0, 0, 0 },
                { 0, 1, 0, 0 },
                { 1, 1, 1, 0 },
                { 0, 0, 0, 0 }
            }
        };
    };
    
    class I : public Interfaceable<I>{
    public:
        I() = default;
        virtual ~I() = default;
        
        Matrix shape{
            {
                { 0, 1, 0, 0 },
                { 0, 1, 0, 0 },
                { 0, 1, 0, 0 },
                { 0, 1, 0, 0 }
            }
        };
    };
    
    class Z : public Interfaceable<Z>{
    public:
        Z() = default;
        virtual ~Z() = default;
        
        Matrix shape{
            {
                { 0, 0, 0, 0 },
                { 0, 1, 1, 0 },
                { 0, 1, 0, 0 },
                { 0, 1, 0, 0 }
            }
        };
    };
};

class NonCopyable{
public:
    NonCopyable() = default;
    virtual ~NonCopyable() = default;
    
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable(const NonCopyable &&) = delete;
    NonCopyable& operator = (const NonCopyable&) = delete;
};

class Tetris : private NonCopyable{
public:
    using Ptr = std::unique_ptr<Shape>;
    
    Tetris();
    
    void moveBlock(std::size_t, std::size_t);
    bool isCollide(std::size_t, std::size_t);
    void spawnBlock();
    bool doRotate();
    bool isFull();
    
    Point2D getPosition(){
        return position;
    }
    
private:
    void nplayWidth();
    void makeBlocks();
    void checkLine();
    void makeSolid();
    
    Matrix mStage;
    
    Point2D position;
    
    Shape *shape;
    
    void draw(std::ostream& stream) const;
    
    friend std::ostream& operator<<(std::ostream& stream, const Tetris& self){
        self.draw(stream);
        return stream;
    }
    
    int blockType = 0;
    int mScore = 0;
    Matrix mBoard;
    
    std::minstd_rand rndEngine;
    
    std::vector<Ptr> shapes;
    
    std::size_t columnSize() const{
        return range(bHeight::CE_FROM, bHeight::CE_TO).size();
    }
    
    std::size_t rowSize() const{
        return range(nRow::RE_FROM, nRow::RE_TO).size();
    }
    
};

Tetris::Tetris(){
    mBoard.resize(columnSize(), std::vector<int>(rowSize(), 0));
    mStage.resize(columnSize(), std::vector<int>(rowSize(), 0));
    
    shapes.emplace_back(std::move(std::make_unique<shapes::T>()->clone()));
    shapes.emplace_back(std::move(std::make_unique<shapes::M>()->clone()));
    shapes.emplace_back(std::move(std::make_unique<shapes::N>()->clone()));
    shapes.emplace_back(std::move(std::make_unique<shapes::I>()->clone()));
    shapes.emplace_back(std::move(std::make_unique<shapes::O>()->clone()));
    shapes.emplace_back(std::move(std::make_unique<shapes::L>()->clone()));
    shapes.emplace_back(std::move(std::make_unique<shapes::Z>()->clone()));
    
    nplayWidth();
}

void Tetris::nplayWidth(){
    for (const auto& i : range(bHeight::CE_FROM, bHeight::CE_TO - 1)) {
        for (const auto& j : range(nRow::RE_FROM, nRow::RE_TO - 1)){
            if ((j == 0) || (j == rowSize() - 2) || (i == columnSize() - 2)){
                mBoard[i][j] = mStage[i][j] = WALL;
            }
            else{
                mBoard[i][j] = mStage[i][j] = EMPTY;
            }
        }
    }
    
    makeBlocks();
}

void Tetris::makeBlocks(){
    position.x = shape->size();
    position.y = 0;
    
    int shapeCounts = 7;
    
    blockType = rndEngine() % shapeCounts;
    
    shape = shapes[blockType].get();
    
    for (const auto& i : range(nblock::FROM, nblock::TO)){
        for (const auto& j : range(nblock::FROM, nblock::TO)){
            mBoard[i][j + shape->size()] += shapes[blockType]->nCoordinate(i, j);
        }
    }
}

bool Tetris::isFull(){
    for (const auto& i : range(nblock::FROM, nblock::TO)){
        for (const auto& j : range(nblock::FROM, nblock::TO)){
            if (mBoard[i][j + shape->size()] > 1){
                return true;
            }
        }
    }
    
    return false;
}
void Tetris::moveBlock(std::size_t x2, std::size_t y2){
    
    for (const auto& i : range(nblock::FROM, nblock::TO)){
        for (const auto& j : range(nblock::FROM, nblock::TO)){
            mBoard[position.y + i][position.x + j] -= shapes[blockType]->nCoordinate(i, j);;
        }
    }
    
    position.x = x2;
    position.y = y2;
    
    for (const auto& i : range(nblock::FROM, nblock::TO)){
        for (const auto& j : range(nblock::FROM, nblock::TO)){
            mBoard[position.y + i][position.x + j] += shapes[blockType]->nCoordinate(i, j);
        }
    }
}

void Tetris::checkLine(){
    std::copy(mBoard.begin(), mBoard.end(), mStage.begin());
    
    for (const auto& i : range(bHeight::CE_FROM + 1, bHeight::CE_TO - 2)){
        bool LineCompletion = true;
        
        for (const auto& j : range(nRow::RE_FROM + 1, nRow::RE_TO - 1)){
            if (mStage[i][j] == 0){
                LineCompletion = false;
            }
        }
        
        if (LineCompletion){
            mScore += 10;
            
            for (const auto& k : range(nblock::FROM, nblock::TO)){
                std::copy(mStage[i - 1 - k].begin(), mStage[i - 1 - k].end(), mStage[i - k].begin());
            }
        }
    }
    
    std::copy(mStage.begin(), mStage.end(), mBoard.begin());
}

bool Tetris::isCollide(std::size_t x, std::size_t y){
    for (const auto& i : range(nblock::FROM, nblock::TO)){
        for (const auto& j : range(nblock::FROM, nblock::TO)){
            if (shapes[blockType]->nCoordinate(i, j) && mStage[y + i][x + j] != 0){
                return true;
            }
        }
    }
    return false;
}

bool Tetris::doRotate(){
    Matrix temp(shape->size(), std::vector<int>(shape->size(), 0));
    
    std::copy(shapes[blockType]->shape().begin(), shapes[blockType]->shape().end(), temp.begin());
    
    shape->rotate();
    
    if (isCollide(position.x, position.y)){
        std::copy(temp.begin(), temp.end(), shapes[blockType]->shape().begin());
        
        return true;
    }
    
    for (const auto& i : range(nblock::FROM, nblock::TO)){
        for (const auto& j : range(nblock::FROM, nblock::TO)){
            mBoard[position.y + i][position.x + j] -= temp[i][j];
            mBoard[position.y + i][position.x + j] += shapes[blockType]->nCoordinate(i, j);
        }
    }
    
    return false;
}

void Tetris::makeSolid(){
    if (isCollide(position.x, position.y + 1)){
        for (const auto& i : range(nblock::FROM, nblock::TO)){
            for (const auto& j : range(nblock::FROM, nblock::TO)){
                if (shapes[blockType]->nCoordinate(i, j) != 0){
                    mBoard[position.y + i][position.x + j] = WALL;
                }
            }
        }
    }
}
void Tetris::spawnBlock(){
    if (!isCollide(position.x, position.y + 1)){
        moveBlock(position.x, position.y + 1);
    }
    else{
        makeSolid();
        checkLine();
        makeBlocks();
    }
}

void Tetris::draw(std::ostream& stream) const{
    for (auto i : mBoard){
        for (auto j : i){
            switch (j){
                case EMPTY:
                    stream << ' ';
                    break;
                case WALL:
                    rlutil::setColor(rlutil::WHITE);
                    stream << 'X';
                    break;
                default:
                    if (blockType == 0){
                        rlutil::setColor(rlutil::GREEN);
                    }
                    else{
                        rlutil::setColor(blockType);
                    }
                    stream << 'O';
                    break;
            }
        }
        
        stream << '\n';
    }
    
    rlutil::setColor(rlutil::GREY);
    
    stream << "Score : " << mScore
    << "\n\narrow keys\nleft: ["
    << static_cast<char>(27) << "]\ndown:["
    << static_cast<char>(25) << "]\nright:["
    << static_cast<char>(26) << "]\nRotation:["
    << static_cast<char>(24) << "]";
}

class Game : private NonCopyable{
public:
    int menu();
    void gameLoop();
    void gameLoop2();
    void gameLoop3();
private:
    void introScreen();
    void userInput();
    void display();
    void gameOverScreen();
    
    Tetris tetris;
};

void Game::gameOverScreen(){
    gotoxy(10, 10);
    rlutil::setColor(rlutil::RED);
    
    std::cout << "\n"
    " ~ Game Over. It's ok, you'll live.~"
    "\n\nPress enter to exit\n";
    
    std::cin.ignore();
    std::cin.get();
}

void Game::gameLoop(){
    auto start = std::chrono::high_resolution_clock::now();
    
    while (!tetris.isFull()){
        auto end = std::chrono::high_resolution_clock::now();
        
        double timeTakenInSeconds = (end - start).count()
        * (static_cast<double>(std::chrono::high_resolution_clock::period::num)
           / std::chrono::high_resolution_clock::period::den);
        
        if (kbhit()){
            userInput();
        }
        
        if (timeTakenInSeconds > 0.5){
            tetris.spawnBlock();
            display();
            start = std::chrono::high_resolution_clock::now();
        }
    }
    
    rlutil::cls();
    
    gameOverScreen();
}

void Game::gameLoop2(){
    auto start = std::chrono::high_resolution_clock::now();
    
    while (!tetris.isFull()){
        auto end = std::chrono::high_resolution_clock::now();
        
        double timeTakenInSeconds = (end - start).count()
        * (static_cast<double>(std::chrono::high_resolution_clock::period::num)
           / std::chrono::high_resolution_clock::period::den);
        
        if (kbhit()){
            userInput();
        }
        
        if (timeTakenInSeconds > 0.05){
            tetris.spawnBlock();
            display();
            start = std::chrono::high_resolution_clock::now();
        }
    }
    
    rlutil::cls();
    
    gameOverScreen();
}

void Game::gameLoop3(){
    auto start = std::chrono::high_resolution_clock::now();
    
    while (!tetris.isFull()){
        auto end = std::chrono::high_resolution_clock::now();
        
        double timeTakenInSeconds = (end - start).count()
        * (static_cast<double>(std::chrono::high_resolution_clock::period::num)
           / std::chrono::high_resolution_clock::period::den);
        
        if (kbhit()){
            userInput();
        }
        
        if (timeTakenInSeconds > 0.0165){
            tetris.spawnBlock();
            display();
            start = std::chrono::high_resolution_clock::now();
        }
    }
    
    rlutil::cls();
    
    gameOverScreen();
}

int Game::menu(){
    
    rlutil::cls();
    std::cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    "\n    ****** Tetris ******\n""~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    "\n\t   *Menu*\n"
    "\t  1: Start\n\t  2: Quit\n\n"
    "~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
    "Choice >> ";
    
    int selectNum = 0;
    
    std::cin >> selectNum;
    
    switch (selectNum){
        case 1: PLAY:
        case 2: EXIT:
            break;
        default:
            selectNum = 0;
            break;
    }
    
    return selectNum;
}

void Game::display(){
    rlutil::cls();
    std::cout << tetris;
}

void Game::userInput(){
    const int k = rlutil::getkey();
    switch (k){
        case rlutil::KEY_RIGHT:
            if (!tetris.isCollide(tetris.getPosition().x + 1, tetris.getPosition().y)){
                tetris.moveBlock(tetris.getPosition().x + 1, tetris.getPosition().y);
            }
            break;
        case rlutil::KEY_LEFT:
            if (!tetris.isCollide(tetris.getPosition().x - 1, tetris.getPosition().y)){
                tetris.moveBlock(tetris.getPosition().x - 1, tetris.getPosition().y);
            }
            break;
        case rlutil::KEY_DOWN:
            if (!tetris.isCollide(tetris.getPosition().x, tetris.getPosition().y + 1)){
                tetris.moveBlock(tetris.getPosition().x, tetris.getPosition().y + 1);
            }
            break;
        case rlutil::KEY_UP:
            tetris.doRotate();
            break;
    }
}

int main(){
    Game game;
    
    switch (game.menu()){
        case PLAY:std::cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~""\n\t*Select Speed*"" \n\t 1: Doable"" \n\t 2: Fast ""\n\t 3: Super Fast"" \n~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nChoice>>";
            std::cin>> selectLevel;
            switch(selectLevel){
                case 1: game.gameLoop();
                case 2: game.gameLoop2();
                case 3: game.gameLoop3();
            }
            break;
        case EXIT:
            return 0;
        default:
            
            return -1;
    }
}

