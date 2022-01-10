//12/24/2021 
//Chess board outline
//Goals:
// 1. Represent pieces in an 8x8 square
// 2. Accept move inputs and keep track of whose move it is
// 3. Add rules: illegal moves, captures, check, checkmate, stalemate,
// en passant, etc.

/**
 * int Board[8][8], where empty squares = 0, and pieces defined below
 * With each piece, only specific moves are possible. Split into "move" 
 * functions for each piece
 * 
 * Have a function for each piece type for possible squares the piece could have come from.
 * For example, return false if there are multiple bishops in different diagonals. Then
 * 
 * Or, maybe keep a separate structure with all pieces on board. Then when piece + destination
 * square is given, directly access the matching pieces and see if any can reach the destination
 * square.
 */
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <array>
#include <unordered_map>
using std::vector;
using std::string;

//enum board_row {A = 1, B = 2, C = 3, D = 4, E = 5, F = 6, G = 7, H = 8};
//Stores piece, color, and corresponding square
struct Piece {
    int piece;
    bool is_white;
    int square; //Location of square in the array. Example:
    //0 = a1, 8 = a2, 16 = a3, 24 = a4, 32 = a5, 40 = a6, 48 = a7, 56 = a8 ... 63 = h8
    //To get column from 0-7, do square / 8. 
    //To get row from 0-7, do square % 8.
};

class Board {
public:
    Board();
    bool Move(string move);
    bool MovePawn(string destination);
    bool MoveKnight(string destination);
    bool MoveBishop(string destination);
    bool MoveRook(string destination);
    bool MoveQueen(string destination);
    bool MoveKing(string destination);
    //Checks all places where a knight could come from, returning true if a knight
    //could reach the destination square from the source square.
    bool FromKnightMoves(int source_square, int destination_square);
    //These functions check all four directions from a destination square, returning
    //true if there is a clear path from the source to the destination.
    //Bottom is where source row < destination row,
    //Left is where source col < destination col
    bool FromBottom(int source_square, int destination_square); 
    bool FromTop(int source_square, int destination_square);
    bool FromLeft(int source_square, int destination_square);
    bool FromRight(int source_square, int destination_square);
    //These functions check both diagonals from a destination square, returning
    //true if there is a clear path from the source to the destination.
    //FromBottomLeftDiag goes from bottom-left of board to destination.
    //Diagonal 1:
    bool FromBottomLeftDiag(int source_square, int destination_square);
    bool FromTopRightDiag(int source_square, int destination_square);
    //Diagonal 2:
    bool FromBottomRightDiag(int source_square, int destination_square);
    bool FromTopLeftDiag(int source_square, int destination_square);
    //Checks if square is within bounds, and has no piece or the piece on the square can be taken
    bool isLegalSquare(string square);
    void PrintBoard();
    void PrintSquares();
    bool WhoseMove();
    //Added after integrating with Qt:
    //Checks if a square has an opponent's piece attacking it (all directions + knights)
    bool NotInCheck(int square, std::array<std::array<int, 8>, 8> &board);
    //Castle kingside and queenside:
    bool CastleKingside();
    bool CastleQueenside();
    bool WhiteInCheck();
    bool BlackInCheck();
    //Print piece locations using the map:
    void PrintPieces();
    void PrintRookAndKingHaveMoved(); //Prints the bools checking if castling is legal
    
private:
    std::array<std::array<int, 8>, 8> board_;
    //int board_[8][8]; //board[row][col]
    //Store locations of all pieces to make it easier to make moves
    //Store all vectors in the piece_map
    //For convenience, kings are also in vectors
    std::unordered_map<int, vector<Piece>*> piece_map;
    vector<Piece> white_pawns_;
    vector<Piece> white_bishops_;
    vector<Piece> white_knights_;
    vector<Piece> white_rooks_;
    vector<Piece> white_queens_;
    vector<Piece> white_king_;
    vector<Piece> black_pawns_;
    vector<Piece> black_knights_;
    vector<Piece> black_bishops_;
    vector<Piece> black_rooks_;
    vector<Piece> black_queens_;
    vector<Piece> black_king_;
    bool white_in_check_;
    bool black_in_check_;
    //Removes the piece on that square from the board and from the appropriate vector.
    void RemovePiece(int square);
    //Moves piece from source (piece's current square) to destination.
    void UpdatePiece(Piece* piece, int destination_square);
    //Store whose turn to move.
    bool white_move;

    //Use a copy board
    bool KingNotInCheckAfterMove(int source_square, int destination_square, std::array<std::array<int, 8>, 8> board);

    //Added after integrating with Qt:
    //Booleans to confirm castling is legal:
    bool white_king_moved_;
    bool black_king_moved_;
    bool white_arook_moved_;
    bool white_hrook_moved_;
    bool black_arook_moved_;
    bool black_hrook_moved_;
    //Helper functions for NotInCheck() that check diagonals, ranks, and files.
    //Returns true if there is a check, false if there is no check
    bool CheckFromBottomLeftDiag(int square, std::array<std::array<int, 8>, 8> &board);
    bool CheckFromTopRightDiag(int square, std::array<std::array<int, 8>, 8> &board);
    bool CheckFromBottomRightDiag(int square, std::array<std::array<int, 8>, 8> &board);
    bool CheckFromTopLeftDiag(int square, std::array<std::array<int, 8>, 8> &board);
    bool CheckFromBottom(int square, std::array<std::array<int, 8>, 8> &board); 
    bool CheckFromTop(int square, std::array<std::array<int, 8>, 8> &board);
    bool CheckFromLeft(int square, std::array<std::array<int, 8>, 8> &board);
    bool CheckFromRight(int square, std::array<std::array<int, 8>, 8> &board);
};
