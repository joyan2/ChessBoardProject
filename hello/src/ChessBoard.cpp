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
    //If not at current position (loaded previous), can't move until loading current position
    if(current_position_ == false) {
        LoadCurrentPosition();
        std::cout << "Current position bool: " << current_position_ <<std::endl;
    }
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
        bool move_success = CastleKingside();
        std::cout << "Reached line " << __LINE__ << std::endl;
        if(move_success && !white_move) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            white_king_moved_ = true;
            white_hrook_moved_ = true;
        } else if(move_success && white_move) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            black_king_moved_ = true;
            black_hrook_moved_ = true;
        }
        if(move_success) {
            last_moved_pawn = nullptr;
            move_++;
            SavePosition();
        }
        return move_success;
    }
    std::cout << "Reached line " << __LINE__ << std::endl;
    if(move == "0-0-0") {
        std::cout << "Reached line " << __LINE__ << std::endl;
        bool move_success = CastleQueenside();
        if(move_success && !white_move) {
            white_king_moved_ = true;
            white_arook_moved_ = true;
        } else if(move_success && white_move) {
            black_king_moved_ = true;
            black_arook_moved_ = true;
        }
        if(move_success) {
            last_moved_pawn = nullptr;
            move_++;
            SavePosition();
        }
        return move_success;
    }
    //If last 2 chars are '=' and a valid piece, try promoting pawn
    //Must be from a valid col to be a pawn move
    if(move.size() >= 4 && IsValidCol(move.at(0)) && CanPromoteInto(move.at(move.size()-1)) && move.at(move.size()-2) == '=') {
        std::cout << "Try promoting" << '\n';
        string movepawn_input = "";
        //Pawn captures to promotion
        if(toupper(move.at(1)) == 'X') {
            //Example: exd8=Q
            if(move.size() != 6) return false;
            string second_half = move.substr(2);
            string square = second_half.substr(0, 2);
            if(!isLegalSquare(square)) return false;
            move = move.substr(0, 1);
            move += second_half;
            movepawn_input = move.substr(0,3); //movepawn input is 3 chars long
        }
        //If no x was removed, MovePawn() argument will be 2 chars long:
        if(movepawn_input.size() != 3) {
            movepawn_input = move.substr(0,2);
        }
        std::cout << "MovePawn() input: " <<movepawn_input <<std::endl;
        bool pawn_move_success = MovePawn(movepawn_input);
        if(pawn_move_success) last_moved_pawn = nullptr;
        //If pawn moved successfully, should be 
        if(pawn_move_success) {
            std::cout << "Pawn move success" << '\n';
            string destination_square = movepawn_input.substr(0, 2);
            std::cout << "destination square: " <<destination_square <<'\n';
            char promote_to_piece = move.at(move.size()-1);
            std::cout << "Promote to: " << promote_to_piece << '\n';
            if(PromotePawn(destination_square, promote_to_piece) == true) {
                move_++;
                SavePosition();
                return true;
            }
        }
        
        return false;
    }
    string square = move.substr(move.size()-2);
    if(!isLegalSquare(square)) return false;
    //If an 'x' is before the square, make sure a capturable piece is on there, otherwise return false. 
    //Remove the 'x' from the string.
    if(move.size() >= 4 && toupper(move.at(move.size()-3)) == 'X') {
        std::cout << "x detected" << '\n';
        //For en passant and pawn captures:
        if(move.size() == 4 && IsValidCol(move.at(0))) {
            std::cout << "reached en passant/pawn captures";
            string destination = square;
            int color_multiplier = white_move ? 1 : (-1);
            char row = destination.at(destination.size() - 2);
            int col = std::stoi(destination.substr(destination.size() - 1));
            int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
            int destination_square = (col-1) * ONE_COL + (destination_row-1);
            //If the column is right and 
            if((white_move && (destination_square / 8) == 5)
            ||(!white_move && (destination_square / 8) == 2)) {
                if(board_[destination_square%8][destination_square/8] == 0
                && CanEnPassant(square)) {
                    std::cout << "pawn can en passant " << move << '\n';
                    move = move.substr(0, move.size()-3);
                    move += square;
                } else if(IsCapturable(square)) {
                    std::cout << "pawn can capture on 6th rank: " << move << '\n';
                    move = move.substr(0, move.size()-3);
                    move += square;
                } else {
                    return false;
                }
            }  else if(IsCapturable(square)) {
                    std::cout << "pawn can capture on x rank: " << move << '\n';
                    move = move.substr(0, move.size()-3);
                    move += square;
                }
        }
        //For other pieces:
        else if(IsCapturable(square)) {
            //Remove the 'x' from move:
            move = move.substr(0, move.size()-3);
            move += square;
            std::cout << "move without x: " << move << '\n';
        } else {
            return false;
        }
    }
    if(toupper(move.at(0)) == 'N') {
        std::cout << "Moving knight" << '\n';
        bool move_success = MoveKnight(move.substr(1));
        if(move_success) {
            last_moved_pawn = nullptr;
            move_++;
            SavePosition();
        }
        return move_success;
    } else if(toupper(move.at(0)) == 'B') {
        bool move_success = MoveBishop(move.substr(1));
        if(move_success) {
            last_moved_pawn = nullptr;
            move_++;
            SavePosition();
        }
        return move_success;
    } else if(toupper(move.at(0)) == 'R') {
        bool move_success = MoveRook(move.substr(1));
        if(move_success) {
            last_moved_pawn = nullptr;
            move_++;
            SavePosition();
        }
        return move_success;
    } else if(toupper(move.at(0)) == 'Q') {
        bool move_success = MoveQueen(move.substr(1));
        if(move_success) {
            last_moved_pawn = nullptr;
            move_++;
            SavePosition();
        }
        return move_success;
    } else if(toupper(move.at(0)) == 'K') {
        bool king_moved = MoveKing(move.substr(1));
        if(king_moved == true && !white_move) {
            black_king_moved_ = true;
        } else if (king_moved == true && white_move) {
            white_king_moved_ = true;
        }
        if(king_moved) {
            last_moved_pawn = nullptr;
            move_++;
            SavePosition();
        }
        return king_moved;
    } else if(move.size() == 3) {
        //Check that first move is between a-h. The other two chars are already
        //confirmed to be a legal square.
        if(IsValidCol(move.at(0))) {
            bool move_success = MovePawn(move);
            if(move_success) {
                move_++;
                SavePosition();
                return true;
            }
        }
    } else if(move.size() == 2) {
        bool move_success = MovePawn(square);
        if(move_success) {
            move_++;
            SavePosition();
            return true;
        }
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
    
    int color_multiplier = white_move ? 1 : (-1);
    //For captures:
    if(destination.size() == 3) {
        if(!IsValidCol(destination.at(0))) return false; //Confirm that first char is valid column
        //If destination square is empty, check for en passant
        if(board_[destination_square%8][destination_square/8] == 0) {
            if(last_moved_pawn == nullptr) return false;
            //If destination is directly behind last moved pawn, check if pawn does en passant
            if(last_moved_pawn->square == destination_square-color_multiplier*8) {
                vector<Piece>* matching_pieces = piece_map[pawn*color_multiplier];
                for(int i = 0; i < matching_pieces->size(); i++) {
                    Piece* p = &matching_pieces->at(i);
                    if(!IsSpecifiedCol(p->square, destination.at(0))) continue;
                    std::cout << "Reached line " << __LINE__ << std::endl;
                    std::array<std::array<int, 8>, 8> board = board_;
                    //std::cout << board[2][5];
                    if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                    RemovePiece(last_moved_pawn->square);
                    UpdatePiece(p, destination_square);
                    last_moved_pawn = nullptr;
                    return true;
                }
            }
        } else {
            vector<Piece>* matching_pieces = piece_map[pawn*color_multiplier];
            for(int i = 0; i < matching_pieces->size(); i++) {
                std::cout << "Capturing with pawn " << std::endl;
                Piece* p = &matching_pieces->at(i);
                if(!IsSpecifiedCol(p->square, destination.at(0))) continue;
                std::cout << "Reached line " << __LINE__ << std::endl;
                std::array<std::array<int, 8>, 8> board = board_;
                //std::cout << board[2][5];
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                RemovePiece(destination_square);
                UpdatePiece(p, destination_square);
                last_moved_pawn = nullptr;
                return true;
            }
        }
    }
    else if(white_move) {
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
                    last_moved_pawn = nullptr;
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
                        last_moved_pawn = p;
                        return true;
                    }
                }
                
            }
        }
        //Rest of cases:
        if(col != 4 && col >= 3 && col <=8) {
            vector<Piece>* matching_pieces = piece_map[pawn];
            for(int i = 0; i < matching_pieces->size(); i++) {
                Piece* p = &matching_pieces->at(i);
                if(p->square == destination_square - ONE_COL) {
                    std::array<std::array<int, 8>, 8> board = board_;
                    if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                    UpdatePiece(p, destination_square);
                    last_moved_pawn = nullptr;
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
                    last_moved_pawn = nullptr;
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
                        last_moved_pawn = p;
                        return true;
                    }
                }
                
            }
        }
        //Rest of cases:
        if(col != 5 && col >= 1 && col <=6) {
            vector<Piece>* matching_pieces = piece_map[-pawn];
            for(int i = 0; i < matching_pieces->size(); i++) {
                Piece* p = &matching_pieces->at(i);
                if(p->square == destination_square + ONE_COL) {
                    std::array<std::array<int, 8>, 8> board = board_;
                    if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                    UpdatePiece(p, destination_square);
                    last_moved_pawn = nullptr;
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
    bool possible_bishop = false; //True if a possible knight is found
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
    vector<Piece>* matching_pieces = piece_map[bishop_color_multiplier*bishop];
    for(int i = 0; i < matching_pieces->size(); i++) {
        Piece* p = &matching_pieces->at(i);
        //Case: BottomRightDiag to TopLeftDiag
                if(col_specified && !IsSpecifiedCol(p->square, destination.at(0))) {
            std::cout << "col doesn't match " << std::endl;
            continue;
        };
        if(row_specified && !IsSpecifiedRow(p->square, destination.at(0))) continue;
        if(square_specified && !IsSpecifiedCol(p->square, destination.at(0))
        && !IsSpecifiedRow(p->square, destination.at(1))) continue;
        if(p->square % 7 == destination_square % 7) {
            if(FromBottomRightDiag(p->square, destination_square) || FromTopLeftDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                if(possible_bishop == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_bishop = true;
                    possible_piece = p;
                }
            }
        }
        //Case: BottomLeftDiag to TopRightDiag
        else if(p->square % 9 == destination_square % 9) {
            if(FromBottomLeftDiag(p->square, destination_square) || FromTopRightDiag(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                if(possible_bishop == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_bishop = true;
                    possible_piece = p;
                }
            }
        }
    }
    if(possible_bishop) {
        RemovePiece(destination_square);
        UpdatePiece(possible_piece, destination_square);
        return true;
    }
    return false;
}

bool Board::MoveRook(string destination) {
    if(destination.size() > 3) return false; //not possible for Rook to have 2 squares specified. 
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
    bool possible_rook = false; //True if a possible knight is found
    Piece* possible_piece = nullptr;
    bool col_specified = (destination.size() == 3 && IsValidCol(destination.at(0)));
    bool row_specified = (destination.size() == 3 && IsValidRow(destination.at(0)));
    //If destination size >= 3 but no valid col, row, or square was specified, return false:
    if(destination.size() == 3 && (!col_specified && !row_specified)) {
        std::cout << "No valid col/row/square specified" << '\n';
        return false;
    }
    vector<Piece>* matching_pieces = piece_map[rook_color_multiplier*rook];
    for(int i = 0; i < matching_pieces->size(); i++) {
        Piece* p = &matching_pieces->at(i);
        //std::cout << p->square << " ";
        std::cout << "Reached line " << __LINE__ << std::endl;
        std::cout << "Rook: " << p->square << '\n';
        std::cout << "Destination square: " << destination_square << '\n';
        if(col_specified && !IsSpecifiedCol(p->square, destination.at(0))) {
            std::cout << "col doesn't match " << std::endl;
            continue;
        };
        if(row_specified && !IsSpecifiedRow(p->square, destination.at(0))) continue;
        if(p->square % 8 == destination_square % 8) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            //TODO: Add case handling multiple possible rooks (include FromLeft and FromRight)
            if(FromBottom(p->square, destination_square) || FromTop(p->square, destination_square)) {
                std::cout << "Reached line " << __LINE__ << std::endl;
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                if(possible_rook == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_rook = true;
                    possible_piece = p;
                }
            }
        }
        else if(p->square / 8 == destination_square / 8) {
            std::cout << "Reached line " << __LINE__ << std::endl;
            if(FromLeft(p->square, destination_square) || FromRight(p->square, destination_square)) {
                std::cout << "Reached line " << __LINE__ << std::endl;
                std::array<std::array<int, 8>, 8> board = board_;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                if(possible_rook == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_rook = true;
                    possible_piece = p;
                }
            }
        }
        
    }
    if(possible_rook) {
        RemovePiece(destination_square);
        UpdatePiece(possible_piece, destination_square);
        return true;
    }
    return false;
}
void Board::RemovePiece(int square) {
    int piece_at_square = board_[square % 8][square / 8];
    if(piece_at_square == 0) return;
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
    //Use of modulo: only need to ensure the resulting knight move does not wrap around rows, since will never
    //wrap around columns.
    if((source_square + 15 == destination_square && (source_square+15)%8 == source_square%8 - 1) 
    || (source_square - 15 == destination_square && (source_square-15)%8 == source_square%8 + 1)
    || (source_square + 17 == destination_square && (source_square+17)%8 == source_square%8 + 1)
    || (source_square - 17 == destination_square && (source_square-17)%8 == source_square%8 - 1)
    || (source_square + 6 == destination_square && (source_square+6)%8 == source_square%8 - 2) 
    || (source_square - 6 == destination_square && (source_square-6)%8 == source_square%8 + 2)
    || (source_square + 10 == destination_square && (source_square+10)%8 == source_square%8 + 2) 
    || (source_square - 10 == destination_square && (source_square-10)%8 == source_square%8 - 2)) {
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
    bool possible_queen = false; //True if a possible knight is found
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
    vector<Piece>* matching_pieces = piece_map[queen_color_multiplier*queen];
    for(int i = 0; i < matching_pieces->size(); i++) {
        Piece* p = &matching_pieces->at(i);
        if(col_specified && !IsSpecifiedCol(p->square, destination.at(0))) {
            std::cout << "col doesn't match " << std::endl;
            continue;
        };
        if(row_specified && !IsSpecifiedRow(p->square, destination.at(0))) continue;
        if(square_specified && !IsSpecifiedCol(p->square, destination.at(0))
        && !IsSpecifiedRow(p->square, destination.at(1))) continue;
        if(p->square % 8 == destination_square % 8) {
            //TODO: Add case handling multiple possible rooks (include FromLeft and FromRight)
            if(FromBottom(p->square, destination_square) || FromTop(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 3 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                if(possible_queen == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_queen = true;
                    possible_piece = p;
                }
            }
        }
        if(p->square / 8 == destination_square / 8) {
            if(FromLeft(p->square, destination_square) || FromRight(p->square, destination_square)) {
                std::array<std::array<int, 8>, 8> board = board_;
                std::cout << "Reached line 4 " << __LINE__ << std::endl;
                if(!KingNotInCheckAfterMove(p->square, destination_square, board)) return false;
                if(possible_queen == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_queen = true;
                    possible_piece = p;
                }
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
                if(possible_queen == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_queen = true;
                    possible_piece = p;
                }
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
                if(possible_queen == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_queen = true;
                    possible_piece = p;
                }
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
                if(possible_queen == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_queen = true;
                    possible_piece = p;
                }
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
                if(possible_queen == true) {
                    std::cout << "Another possible knight found at: ";
                    std::cout << possible_piece->square << std::endl;;
                    return false;
                } else {
                    possible_queen = true;
                    possible_piece = p;
                }
            }
        }

    }
    if(possible_queen) {
        RemovePiece(destination_square);
        UpdatePiece(possible_piece, destination_square);
        return true;
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
    move_ = 0;
    current_position_ = true;
    fifty_move_counter_ = 0;
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
    last_moved_pawn = nullptr;
    SavePosition(); //save the starting position
}
void Board::PrintRookAndKingHaveMoved() {
    std::cout << "White king: " << white_king_moved_ << '\n';
    std::cout << "White a-Rook: " << white_arook_moved_ << '\n';
    std::cout << "White h-Rook: " << white_hrook_moved_ << '\n';
    std::cout << "Black king: " << black_king_moved_ << '\n';
    std::cout << "Black a-Rook: " << black_arook_moved_ << '\n';
    std::cout << "Black h-Rook: " << black_hrook_moved_ << '\n';
}
void Board::SavePosition() {
    Position pos;
    std::cout << "Reached line " << __LINE__ << std::endl;
    pos.board = board_;
    pos.white_pawns = white_pawns_;
    pos.white_bishops = white_bishops_;
    pos.white_knights = white_knights_;
    pos.white_rooks = white_rooks_;
    pos.white_queens = white_queens_;
    pos.white_king = white_king_;
    pos.black_pawns = black_pawns_;
    pos.black_knights = black_knights_;
    pos.black_bishops = black_bishops_;
    pos.black_rooks = black_rooks_;
    pos.black_queens = black_queens_;
    pos.black_king = black_king_;
    std::cout << "Reached line " << __LINE__ << std::endl;
    //Shouldn't need piece_map copy because piece_map in Board points to vectors, no matter
    //how they're updated
    /*pos.piece_map[pawn] = &pos.white_pawns;
    pos.piece_map[knight] = &pos.white_knights;
    pos.piece_map[bishop] = &pos.white_bishops;
    pos.piece_map[rook] = &pos.white_rooks;
    pos.piece_map[queen] = &pos.white_queens;
    pos.piece_map[-pawn] = &pos.black_pawns;
    pos.piece_map[-knight] = &pos.black_knights;
    pos.piece_map[-bishop] = &pos.black_bishops;
    pos.piece_map[-rook] = &pos.black_rooks;
    pos.piece_map[-queen] = &pos.black_queens;
    pos.piece_map[king] = &pos.white_king;
    pos.piece_map[-king] = &pos.black_king;*/
    //If no last moved pawn, set piece to 0
    if(last_moved_pawn == nullptr) {
        pos.last_moved_pawn_square = -1;
    } else {
        pos.last_moved_pawn_square = last_moved_pawn->square;
    }
    std::cout << "Reached line " << __LINE__ << std::endl;
    pos.white_move = white_move;
    //In check:
    pos.white_in_check = white_in_check_;
    pos.black_in_check = black_in_check_;
    //Castling:
    pos.white_king_moved = white_king_moved_;
    pos.black_king_moved = black_king_moved_;
    pos.white_arook_moved = white_arook_moved_;
    pos.white_hrook_moved = white_hrook_moved_;
    pos.black_arook_moved = black_arook_moved_;
    pos.black_hrook_moved = black_hrook_moved_;
    pos.move = move_;
    positions_.push_back(pos);
}
void Board::LoadPosition(int move) {
    if(move < 0 || move >= positions_.size()) return;
    Position* pos = &positions_.at(move);
    board_ = pos->board;
    white_pawns_ = pos->white_pawns;
    white_bishops_ = pos->white_bishops;
    white_knights_ = pos->white_knights;
    white_rooks_ = pos->white_rooks;
    white_queens_ = pos->white_queens;
    white_king_ = pos->white_king;
    black_pawns_ = pos->black_pawns;
    black_knights_ = pos->black_knights;
    black_bishops_ = pos->black_bishops;
    black_rooks_ = pos->black_rooks;
    black_queens_ = pos->black_queens;
    black_king_ = pos->black_king;
    white_move = pos->white_move;
    white_in_check_ = pos->white_in_check;
    black_in_check_ = pos->black_in_check;
    white_king_moved_ = pos->white_king_moved;
    black_king_moved_ = pos->black_king_moved;
    white_arook_moved_ = pos->white_arook_moved;
    white_hrook_moved_ = pos->white_hrook_moved;
    black_arook_moved_ = pos->black_arook_moved;
    black_hrook_moved_ = pos->black_hrook_moved;
    move_ = move;
    //Find the last moved pawn:
    if(pos->last_moved_pawn_square == -1) {
        last_moved_pawn = nullptr;
    } else if(white_move) {
        //Last moved pawn would be black
        for(Piece &pawn : black_pawns_) {
            if(pawn.square == pos->last_moved_pawn_square) {
                last_moved_pawn = &pawn;
                break;
            }
        }
    } else {
        //Last moved pawn would be white
        for(Piece &pawn : white_pawns_) {
            if(pawn.square == pos->last_moved_pawn_square) {
                last_moved_pawn = &pawn;
                break;
            }
        }

    }
    //Update if at current position
    if(move != positions_.size()-1) {
        current_position_ = false;
    } else {
        current_position_ = true;
    }

}
void Board::LoadCurrentPosition() {
    LoadPosition(positions_.size()-1);
}
void Board::TakeBack() {
    TakeBack(1);
}
void Board::TakeBack(int moves) {
    if(moves <= 0) return;
    if(positions_.size() - moves <= 0) {
        std::cout << "Can't take back" << std::endl;
        return;
    }
    LoadPosition(positions_.size()-moves-1);
    for(int i = 0; i < moves; i++) {
        positions_.pop_back();
    }
    current_position_ = true;
}
void Board::PrintVars() {
    std::cout << "Size of positions_ vector: " << positions_.size() << std::endl;
    std::cout << "At current position: " << current_position_ << std::endl;
    PrintPieces();
    std::cout << "Move number: " << move_ << std::endl;
    int last_moved_square = -1;
    if(last_moved_pawn != nullptr) {
        last_moved_square = last_moved_pawn->square;
    }
    std::cout << "Last moved pawn square: " << last_moved_square << std::endl;
    std::cout << "White in check: " << WhiteInCheck() << std::endl;
    std::cout << "Black in check: " << BlackInCheck() << std::endl;
    std::cout << "To move: " << WhoseMove() << std::endl;
    std::cout << "Castling bools: " << white_king_moved_ << " " 
    << black_king_moved_ << " " << white_arook_moved_ << " " << white_hrook_moved_ << " "
    << black_arook_moved_ << " " << black_hrook_moved_ << std::endl;
}
void Board::PrintPieces() {
    for(auto i = piece_map.begin(); i!= piece_map.end(); i++) {
        std::cout << "next " << i->first << std::endl;
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
    int color_multiplier = white_move ? 1 : (-1);

    std::cout << "Is capturable function start destination: " << destination << '\n';
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    int p = board_[destination_square%8][destination_square/8]*color_multiplier;
    return (p != -king && p < 0);
}
bool Board::CanEnPassant(string destination) {
    int color_multiplier = white_move ? 1 : (-1);
    std::cout << "Is capturable en passant start destination: " << destination << '\n';
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    //Destination must be on the 6th rank for the player
    if(!((white_move && (destination_square / 8) == 5)
    ||(!white_move && (destination_square / 8) == 2))) {
        return false;
    }
    //If last moved pawn is not null, last moved pawn is ahead of destination square,
    //en passant is possible. Does not check if a pawn is in the right square
    //to capture it; that is done in MovePawn().
    if(last_moved_pawn != nullptr && last_moved_pawn->square == destination_square - color_multiplier*8) {
        return true;
    }
    return false;
}
bool Board::PromotePawn(string destination, char promote_to_piece) {
    //Having already moved pawn, the pawn should be at string destination. So,
    //change it to the piece specified.
    //Since private function, there must be a pawn at destination.
    //First, change move back because MovePawn() updated whose turn to move:
    if(white_move) {
        white_move = false;
    } else {
        white_move = true;
    }
    char row = destination.at(destination.size() - 2);
    int col = std::stoi(destination.substr(destination.size() - 1));
    int destination_row = toupper(row) - 'A' + 1; //Convert from letter to number, subtract 1
    int destination_square = (col-1) * ONE_COL + (destination_row-1);
    int color_multiplier = white_move ? 1 : (-1);
    vector<Piece>* matching_pieces = piece_map[color_multiplier*pawn];
    for(int i = 0; i < matching_pieces->size(); i++) {
        Piece* p = &matching_pieces->at(i);
        if(p->square == destination_square) {
            std::cout << "Pawn squares matches " << p->square << '\n';
            int ipiece = PieceToInt(promote_to_piece);
            ipiece *= color_multiplier;
            AddPiece(ipiece, destination_square);
            break;
        }
    }
    //Change to_move back
    if(white_move) {
        white_move = false;
    } else {
        white_move = true;
    }
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

int Board::PieceToInt(char piece) {
    switch(piece) {
        case 'N':
            return 2;
        case 'B':
            return 3;
        case 'Q':
            return 9;
        case 'R':
            return 5;
        default:
            return -11;
    }
}

void Board::AddPiece(int piece_value, int square) {
    Piece p;
    p.piece = piece_value;
    p.is_white = white_move;
    p.square = square;
    board_[square%8][square/8] = piece_value;
    piece_map.at(piece_value)->push_back(p);
}

/*
void Board::UpdateMapThroughBoard(std::array<std::array<int, 8>, 8> &board) {
    vector<Piece> white_pawns;
    vector<Piece> white_bishops;
    vector<Piece> white_knights;
    vector<Piece> white_rooks;
    vector<Piece> white_queens;
    vector<Piece> black_pawns;
    vector<Piece> black_knights;
    vector<Piece> black_bishops;
    vector<Piece> black_rooks;
    vector<Piece> black_queens;
    for(int i = 0; i < 8; i++ ) {
        for(int j = 0; j < 8; j ++) {
            switch(board[i][j]) {
                case 0:
                    //std::cout << " 0 ";
                    break;
                case 1: 
                    Piece p;
                    p.piece = pawn;
                    p.is_white = true;
                    p.square = 8 + i;
                    white_pawns.push_back(p);
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
*/