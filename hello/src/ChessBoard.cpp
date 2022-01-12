
#include "ChessBoard.h"
#include <iostream>
#include <iterator>
using std::shared_ptr;
using std::make_shared;

//White is positive, Black is negative for each value.
const int pawn = 1;
const int knight = 2;
const int bishop = 3;
const int rook = 5;
const int queen = 9;
const int king = 10;
const int ONE_COL = 8;
const int ONE_ROW = 1;
bool Board::Move(string move) {
    if(white_in_check_) {
        std::cout << "White king in check" << '\n';
    }
    if(black_in_check_) {
        std::cout << "Black king in check" << '\n';
    }
    if(move.at(move.size()-1) == '+' || move.at(move.size()-1) == '#') {
        //Remove the + or # and continue.
        move = move.substr(0, move.size()-1);
    }
    if(move.size() < 2) return false;
    //std::cout <<square<<std::endl;
    //Depending on first character of string, move the specified piece

    std::cout << "Reached line " << __LINE__ << std::endl;
    if(move == "0-0") {
        std::cout << "Reached line " << __LINE__ << std::endl;
        bool castled_kingside = CastleKingside();
        std::cout << "Reached line " << __LINE__ << std::endl;
        if(castled_kingside && !white_move) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            white_king_moved_ = true;
            white_hrook_moved_ = true;
        } else if(castled_kingside && white_move) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            black_king_moved_ = true;
            black_hrook_moved_ = true;
        }
        return castled_kingside;
    }
    std::cout << "Reached line " << __LINE__ << std::endl;
    if(move == "0-0-0") {
        std::cout << "Reached line " << __LINE__ << std::endl;
        bool castled_queenside = CastleQueenside();
        if(castled_queenside && !white_move) {
            white_king_moved_ = true;
            white_arook_moved_ = true;
        } else if(castled_queenside && white_move) {
            black_king_moved_ = true;
            black_arook_moved_ = true;
        }
        return castled_queenside;
    }
    //If last 2 chars are '=' and a valid piece, try promoting pawn:
    if(move.size() >= 4 && CanPromoteInto(move.at(move.size()-1)) && move.at(move.size()-2) == '=') {
        return PromotePawn(move);
    }
    string square = move.substr(move.size()-2);
    if(!isLegalSquare(square)) return false;
    //If an 'x' is before the square, make sure a capturable piece is on there, otherwise return false. 
    //Remove the 'x' from the string.
    if(move.size() >= 4 && toupper(move.at(move.size()-3)) == 'X') {
        if(IsCapturable(square)) {
            //Remove the 'x' from move:
            move = move.substr(0, move.size()-3);
            move += square;
        } else {
            return false;
        }
    }
    if(toupper(move.at(0)) == 'N') {
        std::cout << "Moving knight" << '\n';
        return MoveKnight(move.substr(1));
    } else if(toupper(move.at(0)) == 'B') {
        return MoveBishop(move.substr(1));
    } else if(toupper(move.at(0)) == 'R') {
        return MoveRook(move.substr(1));
    } else if(toupper(move.at(0)) == 'Q') {
        return MoveQueen(move.substr(1));
    } else if(toupper(move.at(0)) == 'K') {
        bool king_moved = MoveKing(move.substr(1));
        if(king_moved == true && !white_move) {
            black_king_moved_ = true;
        } else if (king_moved == true && white_move) {
            white_king_moved_ = true;
        }
        return king_moved;
    } else if(move.size() == 3) {
        //Check that first move is between a-h. The other two chars are already
        //confirmed to be a legal square.
        if(IsValidCol(move.at(0))) {
            return MovePawn(move);
        }
    } else if(move.size() == 2) {
        return MovePawn(square);
    }
    return false;
}
//Does NOT deal with captures.
bool Board::MovePawn(string destination) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    //Example input: e4.
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    //std::cout << row << " " << col << " " << destination_row << " " << destination_square << std::endl;
    if(white_move) {
        //If column 4, can have moved 2 squares
        if(col == 4) {
            //Check if a piece is in the way
            if(board_[destination_square % 8][destination_square / 8] != pawn && board_[destination_square % 8][destination_square / 8] != 0) return false;
            //Check for pawns 1 and 2 columns directly below the destination
            vector<Piece>* matching_pieces = piece_map[pawn];
            for(int i = 0; i < matching_pieces->size(); i++) {
                Piece* p = &matching_pieces->at(i);
                //std::cout << p->square << std::endl;
                //If pawn one square below destination, update pawn on board
                if(p->square == destination_square - ONE_COL) {
                    std::cout << "Reached line " << __LINE__ << std::endl;
                    std::array<std::array<int, 8>, 8> board = board_;
                    //std::cout << board[2][5];
                    if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                    UpdatePiece(p, destination_square);
                    return true;
                } else if(p->square == destination_square - ONE_COL*2) {
                    //If a pawn above it exists, ignore this pawn and continue
                    if(board_[destination_row-1][col-2] == pawn) {
                        continue;
                    } else {
                        //std::cout << board_[(destination_square - ONE_COL*2) %8][(destination_square - ONE_COL*2)/8] <<std::endl;
                        std::array<std::array<int, 8>, 8> board = board_;
                        if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                        UpdatePiece(p, destination_square);
                        return true;
                    }
                }
                
            }
        }
        //Rest of cases:
        if(col != 4 && col >= 3 && col <=7) {
            vector<Piece>* matching_pieces = piece_map[pawn];
            for(int i = 0; i < matching_pieces->size(); i++) {
                Piece* p = &matching_pieces->at(i);
                if(p->square == destination_square - ONE_COL) {
                    std::array<std::array<int, 8>, 8> board = board_;
                    if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                    UpdatePiece(p, destination_square);
                    return true;
                }
            }
        }


    } 
    //If Black to move:
    else {
        //If column 5, can have moved 2 squares
        if(col == 5) {
            //Check if a piece is in the way
            if(board_[destination_row-1][col-1] != -pawn && board_[destination_row-1][col-1] != 0) return false;
            //Check for pawns 1 and 2 columns directly below the destination
            vector<Piece>* matching_pieces = piece_map[-pawn];
            for(int i = 0; i < matching_pieces->size(); i++) {
                Piece* p = &matching_pieces->at(i);
                //std::cout << p.square << std::endl;
                //If pawn one square below destination, update pawn on board
                if(p->square == destination_square + ONE_COL) {
                    std::array<std::array<int, 8>, 8> board = board_;
                    if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                    UpdatePiece(p, destination_square);
                    return true;
                } else if(p->square == destination_square + ONE_COL*2) {
                    //If a pawn above it exists, ignore this pawn and continue
                    if(board_[destination_row-1][col] == -pawn) {
                        continue;
                    } else {
                        //std::cout << board_[(destination_square - ONE_COL*2) %8][(destination_square - ONE_COL*2)/8] <<std::endl;
                        std::array<std::array<int, 8>, 8> board = board_;
                        if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                        UpdatePiece(p, destination_square);
                        return true;
                    }
                }
                
            }
        }
        //Rest of cases:
        if(col != 5 && col >= 2 && col <=6) {
            vector<Piece>* matching_pieces = piece_map[-pawn];
            for(int i = 0; i < matching_pieces->size(); i++) {
                Piece* p = &matching_pieces->at(i);
                if(p->square == destination_square + ONE_COL) {
                    std::array<std::array<int, 8>, 8> board = board_;
                    if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                    UpdatePiece(p, destination_square);
                    return true;
                }
            }
        }


    }

    std::cout << "Invalid destination" << std::endl;
    return false;
}
bool Board::CanPromoteInto(char piece) {
    return (piece == 'Q' || piece == 'R' || piece == 'B' || piece == 'N');
}
bool Board::MoveKnight(string destination) {
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    int knight_color_multiplier = 0;
    if(white_move) {
        knight_color_multiplier = 1;
    } else {
        knight_color_multiplier = -1;
    }
    bool possible_knight = false; //True if a possible knight is found
    Piece* possible_piece = nullptr;
    bool col_specified = (destination.size() == 3 && IsValidCol(destination.at(0)));
    bool row_specified = (destination.size() == 3 && IsValidRow(destination.at(0)));
    bool square_specified = (destination.size() == 4 && IsValidCol(destination.at(0)
    && IsValidRow(destination.at(1))));
    //If destination size >= 3 but no valid col, row, or square was specified, return false:
    if(destination.size() >= 3 && (!col_specified && !row_specified && !square_specified)) {
        std::cout << "No valid col/row/square specified" << '\n';
        return false;
    }
    std::cout << "col specified: " << col_specified << " " << destination.at(0) << '\n';
    std::cout << "col to int: " <<ColToInt(destination.at(0)) << '\n';
    std::cout << "Destination square: " << destination_square << '\n';
    vector<Piece>* matching_pieces = piece_map[knight_color_multiplier*knight];
    for(int i = 0; i < matching_pieces->size(); i++) {
        std::cout << "knight loop" << std::endl;
        Piece* p = &matching_pieces->at(i);
        std::cout << "Knight square: " << p->square << std::endl;
        //If col, row, or square specified but piece is not on specified col, don't process:
        if(col_specified && !IsSpecifiedCol(p->square, destination.at(0))) {
            std::cout << "col doesn't match " << std::endl;
            continue;
        };
        if(row_specified && !IsSpecifiedRow(p->square, destination.at(0))) continue;
        if(square_specified && !IsSpecifiedCol(p->square, destination.at(0))
        && !IsSpecifiedRow(p->square, destination.at(1))) continue;
        if(FromKnightMoves(p->square, destination_square)) {
            std::cout << "Knight square possible: " << p->square << std::endl;
            std::array<std::array<int, 8>, 8> board = board_;
            if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
            //If another possible knight has been found, don't move:
            if(possible_knight == true) {
                std::cout << "Another possible knight found at: ";
                std::cout << possible_piece->square << std::endl;;
                return false;
            } else {
                possible_knight = true;
                possible_piece = p;
            }
        }
    }
    if(possible_knight) {
        RemovePiece(destination_square);
        UpdatePiece(possible_piece, destination_square);
        return true;
    }
    return false;
}

bool Board::MoveBishop(string destination) {
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    int bishop_color_multiplier = 0;
    if(white_move) {
        bishop_color_multiplier = 1;
    } else {
        bishop_color_multiplier = -1;
    }

    vector<Piece>* matching_pieces = piece_map[bishop_color_multiplier*bishop];
    for(int i = 0; i < matching_pieces->size(); i++) {
        Piece* p = &matching_pieces->at(i);
        //Case: BottomRightDiag to TopLeftDiag
        if(p->square % 7 == destination_square % 7) {
            if(FromBottomRightDiag(p->square, destination_square) || FromTopLeftDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;  
            }
        }
        //Case: BottomLeftDiag to TopRightDiag
        if(p->square % 9 == destination_square % 9) {
            if(FromBottomLeftDiag(p->square, destination_square) || FromTopRightDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;
            }
        }
    }
    return false;
}

bool Board::MoveRook(string destination) {
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    int rook_color_multiplier = 0;
    if(white_move) {
        rook_color_multiplier = 1;
    } else {
        rook_color_multiplier = -1;
    }

    vector<Piece>* matching_pieces = piece_map[rook_color_multiplier*rook];
    for(int i = 0; i < matching_pieces->size(); i++) {
        Piece* p = &matching_pieces->at(i);
        //std::cout << p->square << " ";
        std::cout << "Reached line " << __LINE__ << std::endl;
        std::cout << "Rook: " << p->square << '\n';
        std::cout << "Destination square: " << destination_square << '\n';
        if(p->square % 8 == destination_square % 8) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            //TODO: Add case handling multiple possible rooks (include FromLeft and FromRight)
            if(FromBottom(p->square, destination_square) || FromTop(p->square, destination_square)) {
                std::cout << "Reached line " << __LINE__ << std::endl;
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;
            }
        }
        if(p->square / 8 == destination_square / 8) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            if(FromLeft(p->square, destination_square) || FromRight(p->square, destination_square)) {
                std::cout << "Reached line " << __LINE__ << std::endl;
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;
            }
        }
        
    }
    return false;
}
void Board::RemovePiece(int square) {
    int piece_at_square = board_[square % 8][square / 8];
    vector<Piece>* matching_pieces = piece_map[piece_at_square];
    if(piece_at_square != 0) {
        //Find the matching piece and remove it from the vector
        //Don't need to check if capturable because isLegalSquare() has done so
        for(int i = 0; i < matching_pieces->size(); i++) {
            if(matching_pieces->at(i).square == square) {
                matching_pieces->erase(matching_pieces->begin()+i);
            }
        }
        board_[square % 8][square / 8] = 0;
    }
}
void Board::UpdatePiece(Piece* piece, int destination_square) {
    //Update if rooks have moved from origin
    if(piece->piece == rook) {
        if(piece->square == 7) {
            white_hrook_moved_ = true;
        }
        if(piece->square == 0) {
            white_arook_moved_ = true;
        }
    }
    if(piece->piece == -rook) {
        if(piece->square == 63) {
            black_hrook_moved_ = true;
        }
        if(piece->square == 56) {
            black_arook_moved_ = true;
        }
    }
    board_[destination_square % 8][destination_square / 8] = piece->piece;
    board_[piece->square % 8][piece->square / 8] = 0;
    //std::cout << "curr " << board_[piece->square % 8][piece->square / 8];
    piece->square = destination_square;
    //std::cout << "should be: " << destination_square << std::endl;
    if(white_move) {
        white_move = false;
    } else {
        white_move = true;
    }
    //Update who is in check: opponent's king
    std::cout << "Reached line " << __LINE__ << std::endl;
    if(white_move) {
        if(NotInCheck(white_king_.at(0).square, board_)) {
            white_in_check_ = false;
        } else {
            std::cout << "white in check";
            white_in_check_ = true;
        }
        black_in_check_ = false;
    } else {
        if(NotInCheck(black_king_.at(0).square, board_)) {
            black_in_check_ = false;
        } else {
            std::cout << "black in check";
            black_in_check_ = true;
        }
        white_in_check_ = false;
    }
}

bool Board::FromKnightMoves(int source_square, int destination_square) {
    if(source_square + 15 == destination_square || source_square - 15 == destination_square
    || source_square + 17 == destination_square || source_square - 17 == destination_square
    || source_square + 6 == destination_square || source_square - 6 == destination_square
    || source_square + 10 == destination_square || source_square - 10 == destination_square) {
        return true;
    }
    return false;
}
bool Board::FromBottom(int source_square, int destination_square)  {
    //To be from bottom, source square must be smaller than destination square:
    if(source_square >= destination_square) return false;
    for(int i = source_square + 8; i < destination_square; i += 8) {
        //Note: don't need to check that destination_square is within board or empty/piece capturable since IsLegalSquare() has done so
        if(board_[i%8][i/8] != 0) return false;
    }
    return true;
}
bool Board::FromTop(int source_square, int destination_square) {
    //To be from top, source square must be larger than destination square:
    if(source_square <= destination_square) return false;
    for(int i = source_square - 8; i > destination_square; i -= 8) {
        //Note: don't need to check that destination_square is within board or empty/piece capturable since IsLegalSquare() has done so
        if(board_[i%8][i/8] != 0) return false;
    }
    return true;
}
bool Board::FromLeft(int source_square, int destination_square) {
    //To be from left, source square must be smaller than destination square:
    if(source_square >= destination_square) return false;
    for(int i = source_square + 1; i < destination_square; i++) {
        if(board_[i%8][i/8] != 0) return false;
    }
    //std::cout << "From Left true" << __LINE__ << std::endl;
    return true;
}
bool Board::FromRight(int source_square, int destination_square) {
    //To be from right, source square must be larger than destination square:
    if(source_square <= destination_square) return false;
    //If any pieces in the way, return false:
    for(int i = source_square - 1; i > destination_square; i--) {
        if(board_[i%8][i/8] != 0) return false;
    }
    //std::cout << "From Right true" << __LINE__ << std::endl;
    return true;
}
bool Board::FromBottomLeftDiag(int source_square, int destination_square) {
    //To be from bottom left, source square must be smaller than destination square:
    if(source_square >= destination_square) return false;
    for(int i = source_square + 9; i < destination_square; i += 9) {
        if(board_[i%8][i/8] != 0) {
            std::cout << "test";
            return false;
            
        }
    }
    return true;
}
bool Board::FromTopRightDiag(int source_square, int destination_square) {
    //To be from top right, source square must be larger than destination square:
    if(source_square <= destination_square) return false;
    for(int i = source_square - 9; i > destination_square; i -= 9) {
        if(board_[i%8][i/8] != 0) return false;
    }
    return true;
}
bool Board::FromBottomRightDiag(int source_square, int destination_square) {
    //To be from bottom right, source square must be smaller than destination square:
    if(source_square >= destination_square) return false;
    for(int i = source_square + 7; i < destination_square; i += 7) {
        if(board_[i%8][i/8] != 0) return false;
    }
    return true;
}
bool Board::FromTopLeftDiag(int source_square, int destination_square) {
    //To be from top left, source square must be larger than destination square:
    if(source_square <= destination_square) return false;
    for(int i = source_square - 7; i > destination_square; i -= 7) {
        if(board_[i%8][i/8] != 0) return false;
    }
    return true;
}


bool Board::MoveQueen(string destination) {
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    int queen_color_multiplier = 0;
    if(white_move) {
        queen_color_multiplier = 1;
    } else {
        queen_color_multiplier = -1;
    }

    vector<Piece>* matching_pieces = piece_map[queen_color_multiplier*queen];
    for(int i = 0; i < matching_pieces->size(); i++) {
        Piece* p = &matching_pieces->at(i);
        if(p->square % 8 == destination_square % 8) {
            //TODO: Add case handling multiple possible rooks (include FromLeft and FromRight)
            if(FromBottom(p->square, destination_square) || FromTop(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 3 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;
            }
        }
        if(p->square / 8 == destination_square / 8) {
            if(FromLeft(p->square, destination_square) || FromRight(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 4 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;
            }
        }
        //Case: BottomRightDiag to TopLeftDiag
        bool bottom_right = false;
        int bottom_right_corner = destination_square;
        //while not on the 1st rank or h-file, keep decreasing:
        while(bottom_right_corner > 7 && bottom_right_corner % 8 != 7) {
            if(bottom_right_corner == p->square) {
                bottom_right = true;
                break;
            }
            bottom_right_corner -= 7;
        }
        if(bottom_right_corner == p->square) {
            bottom_right = true;
        }
        if(bottom_right) {
            if(FromBottomRightDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 1 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;  
            }
        }
        //while not on the 8th rank or a-file, keep increasing.
        //Update if the destination square is on the diagonal:
        bool top_left = false;
        int top_left_corner = destination_square;
        while(top_left_corner < 56 && top_left_corner % 8 != 0) {
            if(top_left_corner == p->square) {
                top_left = true;
                break;
            }
            top_left_corner += 7;
        }
        if(top_left_corner == p->square) {
            top_left = true;
        }
        if(top_left) {
            std::cout << "Reached top_left " << __LINE__ << std::endl;
            if(FromTopLeftDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 1 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;  
            }
        }
        //Case: BottomLeftDiag to TopRightDiag
        bool bottom_left = false;
        int bottom_left_corner = destination_square;
        //while not on the 1st rank or a-file, keep decreasing:
        while(bottom_left_corner > 7 && bottom_left_corner % 8 != 0) {
            if(bottom_left_corner == p->square) {
                bottom_left = true;
                break;
            }
            bottom_left_corner -= 9;
        }
        if(bottom_left_corner == p->square) {
            bottom_left = true;
        }
        if(bottom_left) {
            std::cout << "Reached bottom_left " << __LINE__ << std::endl;
            if(FromBottomLeftDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 2 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;
            }
        }
        //while not on the 8th rank or a-file, keep increasing.
        //Update if the destination square is on the diagonal:
        bool top_right = false;
        int top_right_corner = destination_square;
        while(top_right_corner < 56 && top_right_corner % 8 != 7) {
            if(top_right_corner == p->square) {
                top_right = true;
                break;
            }
            top_right_corner += 9;
        }
        if(top_right_corner == p->square) {
            top_right = true;
        }
        if(top_right) {
            std::cout << "Reached top_right " << __LINE__ << std::endl;
            if(FromTopRightDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 2 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                return true;
            }
        }

    }
    return false;
}
bool Board::CastleKingside() {
    if(white_move) {
        std::cout << "Reached line " << __LINE__ << std::endl;
        //If f1 and g1 are clear, king and rook haven't moved, 
        //king not currently in check and king doesn't go through check:
        std::cout << (board_[5][0] == 0) << '\n';
        std::cout << (board_[6][0] == 0) << '\n';
        if(board_[5][0] == 0 && board_[6][0] == 0
        && !white_king_moved_ && !white_hrook_moved_
        && !white_in_check_ && NotInCheck(5, board_)
        && NotInCheck(6, board_)) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            std::cout << "white king at: " << white_king_.at(0).square << '\n';
            UpdatePiece(&white_king_.at(0), 6);
            std::cout << "white king at: " << white_king_.at(0).square << '\n';
            white_move = true; //necessary because UpdatePiece changes this to false
            Piece* r;
            for(Piece rook : white_rooks_) {
                if(rook.square == 7) {
                    r = &rook;
                    UpdatePiece(r, 5);
                    return true;
                }
            }
        }
    }
    else {
        //If f8 and g8 are clear, king and rook haven't moved, 
        //king not currently in check and king doesn't go through check:
        if(board_[5][7] == 0 && board_[6][7] == 0
        && !black_king_moved_ && !black_hrook_moved_
        && !black_in_check_ && NotInCheck(61, board_)
        && NotInCheck(62, board_)) {
            UpdatePiece(&black_king_.at(0), 62);
            white_move = false; //necessary because UpdatePiece changes this to false
            Piece* r;
            for(Piece rook : black_rooks_) {
                if(rook.square == 63) {
                    r = &rook;
                    UpdatePiece(r, 61);
                    return true;
                }
            }
        }
    }
    return false;
}
bool Board::CastleQueenside() {
    if(white_move) {
        //If b1, c1, and d1 are clear, king and rook haven't moved, 
        //king not currently in check and king doesn't go through check:
        if(board_[1][0] == 0 && board_[2][0] == 0 && board_[3][0] == 0
        && !white_king_moved_ && !white_arook_moved_
        && !white_in_check_
        && NotInCheck(2, board_) && NotInCheck(3, board_)) {
            UpdatePiece(&white_king_.at(0), 2);
            white_move = true; //necessary because UpdatePiece changes this to false
            Piece* r;
            for(Piece rook : white_rooks_) {
                if(rook.square == 0) {
                    r = &rook;
                    UpdatePiece(r, 3);
                    return true;
                }
            }
        }
    }
    else {
        //If b8, c8, and d8 are clear, king and rook haven't moved, 
        //king not currently in check and king doesn't go through check:
        if(board_[1][7] == 0 && board_[2][7] == 0 && board_[3][7] == 0
        && !black_king_moved_ && !black_arook_moved_
        && !black_in_check_
        && NotInCheck(58, board_) && NotInCheck(59, board_)) {
            UpdatePiece(&black_king_.at(0), 58);
            white_move = false; //necessary because UpdatePiece changes this to false
            Piece* r;
            for(Piece rook : black_rooks_) {
                if(rook.square == 56) {
                    r = &rook;
                    UpdatePiece(r, 59);
                    return true;
                }
            }
        }
    }
    return false;
}
bool Board::WhiteInCheck() {
    return white_in_check_;
}
bool Board::BlackInCheck() {
    return black_in_check_;
}
bool Board::MoveKing(string destination) {
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);

    //Check if destination square is not in check (even if capturing piece on that square)
    if(!NotInCheck(destination_square, board_)) return false;
    int king_color_multiplier = 0;
    if(white_move) {
        king_color_multiplier = 1;
    } else {
        king_color_multiplier = -1;
    }
    vector<Piece>* matching_pieces = piece_map[king_color_multiplier*king];
    //std::cout << matching_pieces->at(0).piece;
    Piece* p = &matching_pieces->at(0);
    if(p->square - 1 == destination_square || p->square + 1 == destination_square || p->square + 7 == destination_square
    || p->square - 7 == destination_square || p->square + 8 == destination_square || p->square - 8 == destination_square
    || p->square + 9 == destination_square || p->square - 9 == destination_square) {
        RemovePiece(destination_square);
        UpdatePiece(p, destination_square);
        return true;
    }
    
    return false;
}
bool Board::WhoseMove() {
    return white_move;
}
bool Board::KingNotInCheckAfterMove(int source_square, int destination_square, std::array<std::array<int, 8>, 8> board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    board[destination_square%8][destination_square/8] = board[source_square%8][source_square/8];
    board[source_square%8][source_square/8] = 0;
    std::cout << "Reached line " << __LINE__ << std::endl;
    int king_color_multiplier = 0;
    if(white_move) {
        king_color_multiplier = 1;
    } else {
        king_color_multiplier = -1;
    }
    int king_square;
    //get location of player's King
    std::cout << "Reached line " << __LINE__ << std::endl;
    if(board[destination_square%8][destination_square/8] == king_color_multiplier*king) {
        king_square = destination_square;
    } else {
        king_square = piece_map[king_color_multiplier*king]->at(0).square;
    }
    std::cout << "Reached line " << __LINE__ << std::endl;
    return NotInCheck(king_square, board);
    
}
bool Board::NotInCheck(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    //Check knight moves:
    int knight_squares[8]; //squares for the knight
    knight_squares[0] = square + 15;
    knight_squares[1] = square - 15;
    knight_squares[2] = square + 17;
    knight_squares[3] = square - 17;
    knight_squares[4] = square + 6;
    knight_squares[5] = square - 6;
    knight_squares[6] = square + 10;
    knight_squares[7] = square - 10;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }
    //Go through each knight square. If an opponent's knight is on square, return false
    for(int knight_square : knight_squares) {
        if(knight_square < 0 || knight_square > 63) continue;
        if(board[knight_square%8][knight_square/8] == color_multiplier*knight) return false;
    }
    //Check if opponent's king is next to the square, or if opponent's pawn is attacking:
    int king_squares[8];
    king_squares[0] = square - 1;
    king_squares[1] = square + 1;
    king_squares[2] = square + 7;
    king_squares[3] = square - 7;
    king_squares[4] = square + 8;
    king_squares[5] = square - 8;
    king_squares[6] = square + 9;
    king_squares[7] = square - 9;
    for(int i = 0; i < 8; ++i) {
        if(king_squares[i] < 0 || king_squares[i] > 63) continue;
        if(board[king_squares[i]%8][king_squares[i]/8] == color_multiplier*king) return false;
        //Check black pawns if white to move:
        if(white_move && (i == 2 || i == 6) && board[king_squares[i]%8][king_squares[i]/8] == color_multiplier*pawn) return false;
        //std::cout << "pawn";
        //Check white pawns if black to move:
        if((!white_move) && (i == 3 || i == 7) && board[king_squares[i]%8][king_squares[i]/8] == color_multiplier*pawn) return false;
    }
    //Check if opponent's bishop or queen is attacking through a diagonal.
    //Also check if opponent's rook or queen is attacking through a rank or file:
    //std::cout << "checking rank file";
    std::cout << "Reached line " << __LINE__ << std::endl;
    if(CheckFromBottom(square, board) || CheckFromTop(square, board) || CheckFromLeft(square, board) || CheckFromRight(square, board)
    || CheckFromBottomLeftDiag(square, board) || CheckFromBottomRightDiag(square, board)
    || CheckFromTopLeftDiag(square, board) || CheckFromTopRightDiag(square, board)) return false;
    
    std::cout << "Reached line " << __LINE__ << std::endl;
    return true;
}
bool Board::isLegalSquare(string square) {
    //Row and col should exist only from 1-8
    //std::cout << square;
    int row_to_int = toupper(square.at(0)) - 'A' + 1;
    //std::cout << row_to_int <<std::endl;
    int col = std::stoi(square.substr(1));
    //std::cout << col;
    if(row_to_int <= 0 || row_to_int > 8 || col <= 0 || col > 8) {
        std::cout << "out of bounds";
        return false;
    }
    int color_multiplier = 0;
    //Get the opposite color because that means piece is capturable
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }
    int destination_square = board_[row_to_int-1][col-1];
    if(!(destination_square == 0 || destination_square == color_multiplier * pawn || destination_square == color_multiplier * bishop
    || destination_square == color_multiplier * knight || destination_square == color_multiplier * rook || destination_square == color_multiplier * queen)) {
        std::cout << "your own piece is in the way!" << std::endl;
        return false;
    }
    return true;
}
bool Board::CheckFromBottomLeftDiag(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    int i = square - 9;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }
    std::cout << "Reached line " << __LINE__ << std::endl;
    if(i%8 == 0) return false; //If already on bottom left, no check possible
    while((i >= 0 && i <= 63) && i%8 >= 0) {
        std::cout << "Reached line " << __LINE__ << std::endl;
        //If same-color piece, can't be in check:
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored bishop/queen, player is in check:
        if(board[i%8][i/8] == color_multiplier * bishop
        || board[i%8][i/8] == color_multiplier * queen) return true;
        if(i%8 == 0) return false;
        i -= 9;
    }
    std::cout << "Reached line " << __LINE__ << std::endl;
    return false;
}
bool Board::CheckFromTopRightDiag(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    int i = square + 9;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }
    if(i%8 == 7) return false; //If already on bottom right, no check possible
    while((i >= 0 && i <= 63) && i%8 <= 7) {
        //If same-color piece, can't be in check:
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored piece, player is in check:
        if(board[i%8][i/8] == color_multiplier * bishop
        || board[i%8][i/8] == color_multiplier * queen) return true;
        if(i%8 == 7) return false;
        i += 9;
    }
    return false;
}
bool Board::CheckFromBottomRightDiag(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    int i = square - 7;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }
    if(i%8 == 7) return false; //If already on bottom right, no check possible
    while((i >= 0 && i <= 63) && i%8 <= 7) {
        //If same-color piece, can't be in check:
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored piece, player is in check:
        if(board[i%8][i/8] == color_multiplier * bishop
        || board[i%8][i/8] == color_multiplier * queen) return true;
        if(i%8 == 7) return false;
        i -= 7;
    }
    return false;
}
bool Board::CheckFromTopLeftDiag(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    int i = square + 7;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }
    if(i%8 == 0) return false; //If already on top left, no check possible
    while((i >= 0 && i <= 63) && i%8 >= 0) {
        //If same-color piece, can't be in check:
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored piece, player is in check:
        if(board[i%8][i/8] == color_multiplier * bishop
        || board[i%8][i/8] == color_multiplier * queen) {
            std::cout << i << " ";
            std::cout << "Reached line " << __LINE__ << std::endl;
            return true;
        }
        if(i%8 == 0) return false;
        i += 7;
    }
    return false;
}
bool Board::CheckFromBottom(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    std::cout << "square: " << square << '\n';
    int file = square % 8;
    int i = square - 8;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }
    std::cout << "i: " << i <<'\n';
    std::cout << "file: " << file << '\n';
    while((i >= 0 && i <= 63) && i % 8 == file) {
        //If out of bounds, can't be in check:
        if(i < 0 || i > 63) {
            std::cout << color_multiplier * board[i%8][i/8] << '\n';
            return false;
        }
        //If same-color piece, can't be in check:
        std::cout << color_multiplier * board[i%8][i/8] << '\n';
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored piece, player is in check:
        if(board[i%8][i/8] == color_multiplier * rook
        || board[i%8][i/8] == color_multiplier * queen) return true;
        i -= 8;
    }
    return false;
}
bool Board::CheckFromTop(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    int file = square % 8;
    int i = square + 8;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }

    while((i >= 0 && i <= 63) && i % 8 == file) {
        //If out of bounds, can't be in check:
        if(i < 0 || i > 63) return false;
        //If same-color piece, can't be in check:
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored piece, player is in check:
        if(board[i%8][i/8] == color_multiplier * rook
        || board[i%8][i/8] == color_multiplier * queen) return true;
        i += 8;
    }
    return false;
}
bool Board::CheckFromLeft(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    int rank = square / 8;
    int i = square - 1;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }

    while((i >= 0 && i <= 63) && i / 8 == rank) {
        //If out of bounds, can't be in check:
        if(i < 0 || i > 63) return false;
        //If same-color piece, can't be in check:
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored piece, player is in check:
        if(board[i%8][i/8] == color_multiplier * rook
        || board[i%8][i/8] == color_multiplier * queen) {
            std::cout << board[i%8][i/8] << std::endl;
            return true;
        }
        i--;
    }
    return false;
}
bool Board::CheckFromRight(int square, std::array<std::array<int, 8>, 8> &board) {
    std::cout << "Reached line " << __LINE__ << std::endl;
    int rank = square / 8;
    int i = square + 1;
    int color_multiplier = 0;
    if(white_move) {
        color_multiplier = -1;
    } else {
        color_multiplier = 1;
    }

    while((i >= 0 && i <= 63) && i / 8 == rank) {
        //If out of bounds, can't be in check:
        if(i < 0 || i > 63) return false;
        //If same-color piece, can't be in check:
        if(color_multiplier * board[i%8][i/8] < 0) return false;
        //if opposite-colored piece, player is in check:
        if(board[i%8][i/8] == color_multiplier * rook
        || board[i%8][i/8] == color_multiplier * queen) return true;
        i++;
    }
    return false;
}
//Default chessboard setup.
/**
 * 
 */
Board::Board() {
    white_pawns_.reserve(8);
    white_knights_.reserve(2);
    white_bishops_.reserve(2);
    white_rooks_.reserve(2);
    black_pawns_.reserve(8);
    black_knights_.reserve(2);
    black_bishops_.reserve(2);
    black_rooks_.reserve(2);
    for(int i = 0; i < 8; i++) {
        //small array, so iterating through rows should be okay
        board_[i][1] = pawn;
        Piece p;
        p.piece = pawn;
        p.is_white = true;
        p.square = 8 + i;
        white_pawns_.push_back(p);
        board_[i][6] = -pawn; 
        Piece pb;
        pb.piece = -pawn;
        pb.is_white = false;
        pb.square = 48 + i;
        black_pawns_.push_back(pb);
        //Add rooks
        if(i == 0 || i == 7) {
            board_[i][0] = rook;
            Piece pr;
            pr.piece = rook;
            pr.is_white = true;
            pr.square = i;
            white_rooks_.push_back(pr);

            board_[i][7] = -rook;
            Piece prb;
            prb.piece = -rook;
            prb.is_white = false;
            prb.square = 56 + i;
            black_rooks_.push_back(prb);   
        }
        //Add knights
        if(i == 1 || i == 6) {
            board_[i][0] = knight;
            Piece pn; 
            pn.piece = knight;
            pn.is_white = true;
            pn.square = i;
            white_knights_.push_back(pn);

            board_[i][7] = -knight;
            Piece pnb;
            pnb.piece = -knight;
            pnb.is_white = false;
            pnb.square = 56 + i;
            black_knights_.push_back(pnb);
        }
        //Add bishops
        if(i == 2 || i == 5) {
            board_[i][0] = bishop;
            Piece pbw;
            pbw.piece = bishop;
            pbw.is_white = true;
            pbw.square = i;
            white_bishops_.push_back(pbw);

            board_[i][7] = -bishop;
            Piece pbb;
            pbb.piece = -bishop;
            pbb.is_white = false;
            pbb.square = 56 + i;
            black_bishops_.push_back(pbb);
        }
        //Add queens
        if(i == 3) {
            board_[i][0] = queen;
            Piece pq;
            pq.piece = queen;
            pq.is_white = true;
            pq.square = i;
            white_queens_.push_back(pq);

            board_[i][7] = -queen;
            Piece pqb;
            pqb.piece = -queen;
            pqb.is_white = false;
            pqb.square = 56 + i;
            black_queens_.push_back(pqb);
        }
        //Add kings
        if(i == 4) {
            board_[i][0] = king;
            Piece pkw;
            pkw.piece = king;
            pkw.is_white = true;
            pkw.square = i;
            white_king_.push_back(pkw);

            board_[i][7] = -king;
            Piece pkb;
            pkb.piece = -king;
            pkb.is_white = false;
            pkb.square = 56+i;
            black_king_.push_back(pkb);
        }
    }
    //Fill up the rest of the board with 0s
    for(int i = 0; i < 8; i++) {
        for(int j = 2; j < 6; j++) {
            board_[i][j] = 0;
        }
    }
    //Add all vectors to the map 
    piece_map[pawn] = &white_pawns_;
    piece_map[knight] = &white_knights_;
    piece_map[bishop] = &white_bishops_;
    piece_map[rook] = &white_rooks_;
    piece_map[queen] = &white_queens_;
    piece_map[-pawn] = &black_pawns_;
    piece_map[-knight] = &black_knights_;
    piece_map[-bishop] = &black_bishops_;
    piece_map[-rook] = &black_rooks_;
    piece_map[-queen] = &black_queens_;
    piece_map[king] = &white_king_;
    piece_map[-king] = &black_king_;
    white_move = true; //White to move.
    white_in_check_ = false;
    black_in_check_ = false;
    white_king_moved_ = false;
    black_king_moved_ = false;
    white_arook_moved_ = false;
    white_hrook_moved_ = false;
    black_arook_moved_ = false;
    black_hrook_moved_ = false;
}
void Board::PrintRookAndKingHaveMoved() {
    std::cout << "White king: " << white_king_moved_ << '\n';
    std::cout << "White a-Rook: " << white_arook_moved_ << '\n';
    std::cout << "White h-Rook: " << white_hrook_moved_ << '\n';
    std::cout << "Black king: " << black_king_moved_ << '\n';
    std::cout << "Black a-Rook: " << black_arook_moved_ << '\n';
    std::cout << "Black h-Rook: " << black_hrook_moved_ << '\n';
}
void Board::PrintPieces() {
    for(auto i = piece_map.begin(); i!= piece_map.end(); i++) {
        for(int j = 0; j < (i->second)->size(); j++) {
            std::cout << "piece: ";
            switch((i->second)->at(j).piece) {
                case 0:
                    std::cout << " 0 ";
                    break;
                case 1: 
                    std::cout << " P ";
                    break;
                case 2:
                    std::cout << " N ";
                    break;
                case 3: 
                    std::cout << " B ";
                    break;
                case 5:
                    std::cout << " R ";
                    break;
                case 9: 
                    std::cout << " Q ";
                    break;
                case 10:
                    std::cout << " K ";
                    break;
                case -1: 
                    std::cout << "-P ";
                    break;
                case -2:
                    std::cout << "-N ";
                    break;
                case -3: 
                    std::cout << "-B ";
                    break;
                case -5:
                    std::cout << "-R ";
                    break;
                case -9: 
                    std::cout << "-Q ";
                    break;
                case -10:
                    std::cout << "-K ";
                    break;
            }
            std::cout << "square: " << (i->second)->at(j).square << " ";
        }
        std::cout << '\n';
    }
}
void Board::PrintBoard() {
    for(int i = 0; i < 8; i++ ) {
        for(int j = 0; j < 8; j ++) {
            switch(board_[i][j]) {
                case 0:
                    std::cout << " 0 ";
                    break;
                case 1: 
                    std::cout << " P ";
                    break;
                case 2:
                    std::cout << " N ";
                    break;
                case 3: 
                    std::cout << " B ";
                    break;
                case 5:
                    std::cout << " R ";
                    break;
                case 9: 
                    std::cout << " Q ";
                    break;
                case 10:
                    std::cout << " K ";
                    break;
                case -1: 
                    std::cout << "-P ";
                    break;
                case -2:
                    std::cout << "-N ";
                    break;
                case -3: 
                    std::cout << "-B ";
                    break;
                case -5:
                    std::cout << "-R ";
                    break;
                case -9: 
                    std::cout << "-Q ";
                    break;
                case -10:
                    std::cout << "-K ";
                    break;
            }
        }
        std::cout << '\n';
    }

}

void Board::PrintSquares() {
    for(int i = 0; i <8; i++) {
        for(int j = 0; j < 8; j++) {
            int square = i + j*8;
            std::cout << square << ' ';
            if(square < 10) {
                std::cout << ' ';
            }
        }
        std::cout << '\n';
    }
}
bool Board::IsCapturable(string destination) {
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    int color_multiplier = white_move ? (-1) : 1;
    int p = board_[destination_square%8][destination_square/8]*color_multiplier;
    return (p != -king && p < 0);
}
bool Board::PromotePawn(string destination) {
    return false;
}
bool Board::IsValidCol(char col) {
    return (tolower(col) >= 'a' && tolower(col) <= 'h');
}
bool Board::IsValidRow(char row) {
    int irow = row - '0'; //convert to int
    return (irow >= 1 && irow <= 8);
}
int Board::ColToInt(char col) {
    return (toupper(col) - 'A');
}
bool Board::IsSpecifiedCol(int square, char col) {
    int icol = ColToInt(col);
    std::cout << "icol: " << icol << '\n';
    std::cout << "square: " << square << '\n';
    return square % 8 == icol;
}
bool Board::IsSpecifiedRow(int square, char row) {
    int irow = row - '0';
    //user-specified row will be 1 to 8, so subtract 1:
    return square / 8 == irow -1 ;
}