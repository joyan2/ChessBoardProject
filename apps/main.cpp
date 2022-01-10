#include "ChessBoard.h"
#include <iostream>

int main() {
    Board b;
    b.PrintSquares();
    bool not_stop = true;
    while(not_stop) {
        char s;
        std::cout << "Input: ";
        std::cin >> s;
        s = tolower(s);
        std::cout << '\n';
        string move = "";
        switch(s) {
            case 'm':
                std::cout << "Enter move: ";
                std::cin >> move;
                b.Move(move);
                break;
            case 'p':
                b.PrintBoard();
                break;
            case 's':
                b.PrintPieces();
                break;
            case 'q':
                std::cout <<"Stopping ";
                not_stop = false;
                break;
            case 'w':
                std::cout << b.WhoseMove() << std::endl;
                break;
            case 'c':
                std::cout << "White in check: " << b.WhiteInCheck() << std::endl;
                std::cout << "Black in check: " << b.BlackInCheck() << std::endl;
            default:
                std::cout << "Try again" << std::endl;
                break;
                
        }
    }
    
    b.PrintBoard();
    b.Move("e4");
    b.PrintBoard();
    b.Move("e5");
    b.PrintBoard();
    b.Move("e5");
    return 0;
}