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
//Position with a board_[][] representation, as well as all necessary bool values
struct Position {
    std::array<std::array<int, 8>, 8> board;
    //Vectors of pieces:
    //std::unordered_map<int, vector<Piece>*> piece_map;
    vector<Piece> white_pawns;
    vector<Piece> white_bishops;
    vector<Piece> white_knights;
    vector<Piece> white_rooks;
    vector<Piece> white_queens;
    vector<Piece> white_king;
    vector<Piece> black_pawns;
    vector<Piece> black_knights;
    vector<Piece> black_bishops;
    vector<Piece> black_rooks;
    vector<Piece> black_queens;
    vector<Piece> black_king;
    
    //Use the square to avoid pointers:
    int last_moved_pawn_square;
    bool white_move;
    //In check:
    bool white_in_check;
    bool black_in_check;
    //Castling:
    bool white_king_moved;
    bool black_king_moved;
    bool white_arook_moved;
    bool white_hrook_moved;
    bool black_arook_moved;
    bool black_hrook_moved;

    //Move number:
    int move;
};

class Board {
public:
    Board();
    bool Move(string move);
    void SavePosition(); //saves the current position in Position struct
    void LoadPosition(int move); //loads the position from saved positions
    void LoadCurrentPosition(); //loads the final position in positions_
    void TakeBack();
    void TakeBack(int moves); //Takes back by a certain number of moves
    bool MovePawn(string destination);
    bool PromotePawn(string destination, char promote_to_piece);
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
    
    void PrintVars(); //for debugging, print private variables
    bool ClaimDraw(); //Claim a draw if three-move or fifty-move
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
    bool RemovePiece(int square);
    //Moves piece from source (piece's current square) to destination.
    void UpdatePiece(Piece* piece, int destination_square);
    //Adds a piece to the specified square. Assume the square is empty.
    void AddPiece(int piece_value, int square);
    //Store whose turn to move.
    bool white_move;

    //Helper function: checks if a char is equal to 'Q', 'R,' 'B', or 'N'
    bool CanPromoteInto(char piece);
    //Helper function: converts a char to the equivalent piece value (positive).
    int PieceToInt(char piece);
    //Checks if a char tolower is between 'a' and 'h' inclusive.
    bool IsValidCol(char col);
    //Checks if a char to int is between 1 and 9 inclusive.
    bool IsValidRow(char row);
    //Checks if a piece is on specified rank(row) or file(col):
    bool IsSpecifiedCol(int square, char col);
    bool IsSpecifiedRow(int square, char row);
    //Converts a char from 'a' to 'h' to the appropriate int from 0 to 7
    int ColToInt(char col);
    //Checks if destination square has an opposite-colored piece that isn't a king
    bool IsCapturable(string destination);
    //Checks if en passant is available
    bool CanEnPassant(string destination);
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
    /**
     * Checks the latest saved position. Goes backwards until 2 more identical positions
     * have been found, or until castling booleans have changed, or until a pawn has moved.
     * From Wikipedia: "Two positions are by definition "the same" if the same types of pieces occupy the 
     * same squares, the same player has the move, the remaining castling rights are the same 
     * and the possibility to capture en passant is the same."
     */
    bool game_end_;
    bool IsThreeMoveRepetition();
    bool IsCheckmate(); //Checks that the person to move is in checkmate
    bool IsStalemate();

    //Converts a string of size 2 into the appropriate int square if possible, -1 otherwise
    int SquareStrToInt(string square);
    //void UpdateMapThroughBoard(std::array<std::array<int, 8>, 8> &board);
    //Pointer to last moved pawn. Used to simplify en passant logic
    Piece* last_moved_pawn;
    vector<Position> positions_;
    int fifty_move_counter_; //Note: has to reach 100
    int move_;
    bool current_position_; //tracks if at current position
};
