#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <functional>

#define SQUARES_NUM 64
#define MOVE_ATRS 4

typedef std::string str;
using namespace std;

typedef array<int, MOVE_ATRS> Move; // from file, from rank, to file, to rank

// pos values will be numbers of 6 bits -> 3.File, 3.Rank

class Board
{

private:
    const int None = 0;
    const int King = 1;
    const int Pawn = 2;
    const int Knight = 3;
    const int Bishop = 4;
    const int Rook = 5;
    const int Queen = 6;

    const int White = 1;
    const int Black = 0;

    bool KingSideCastleW = false;
    bool KingSideCastleB = false;
    bool QueenSideCastleW = false;
    bool QueenSideCastleB = false;

    int KingPosW;
    int KingPosB;

    int Turn = White;

    std::map<char, int>
        pieceMap = {
            {'b', Bishop},
            {'n', Knight},
            {'r', Rook},
            {'q', Queen},
            {'k', King},
            {'p', Pawn}};

    queue<Move> movesMade;

public:
    str fen;
    int board[SQUARES_NUM] = {None};

    Board(str fen)
    {
        this->fen = fen;

        GeneratePositionFromFen(fen);
    }

    void GeneratePositionFromFen(str fen)
    {
        size_t spacePos = fen.find(" ");
        if (spacePos == string::npos)
            return;
        str squareLocations = fen.substr(0, spacePos);

        size_t nextSpacePos = fen.find(" ", spacePos + 1);
        if (nextSpacePos == string::npos)
            return;
        string turn = fen.substr(spacePos + 1, nextSpacePos - spacePos - 1);
        Turn = (turn == "w") ? White : Black;

        spacePos = nextSpacePos;
        nextSpacePos = fen.find(" ", spacePos + 1);
        if (nextSpacePos == string::npos)
            return;
        string castleRights = fen.substr(spacePos + 1, nextSpacePos - spacePos - 1);

        KingSideCastleW = castleRights.find('K') != string::npos;
        KingSideCastleB = castleRights.find('k') != string::npos;
        QueenSideCastleW = castleRights.find('Q') != string::npos;
        QueenSideCastleB = castleRights.find('q') != string::npos;

        int rank = 7;
        int file = 0;

        for (char item : squareLocations)
        {

            if (item == '/')
            {
                rank--;
                file = 0;
            }
            else if ((int)item > 48 && (int)item < 58)
            {
                file += (int)item - 48;
            }
            else if ((int)item > 64 && (int)item < 123)
            {
                if (tolower(item) == 'k')
                {
                    if (isupper(item))
                    {
                        KingPosW = file * 10 + rank;
                    }
                    else
                    {
                        KingPosB = file * 10 + rank;
                    }
                }

                int piece = pieceMap[tolower(item)] * 10 + (isupper(item) ? White : Black);
                int index = 8 * rank + file;
                board[index] = piece;
                file++;
            }
        }
    }

    void DisplayBoard(int newBoard[SQUARES_NUM], int ignore)
    {

        if (ignore) newBoard = board;

        int num = 8;
        int x = 0;
        for (int i = 0; i < SQUARES_NUM; i++)
        {
            int square = newBoard[SQUARES_NUM - num + x];
            str space = square >= 10 ? "  " : "   ";
            x++;

            cout << square << space;
            if ((i + 1) % 8 == 0)
            {
                num += 8;
                x = 0;
                cout << endl;
            }
        }
        cout << "\n\n\n";
    }

    void GetRookMoves(vector<int> &moves, int pos, int newBoard[SQUARES_NUM])
    {
        const int file = GetFileFromPos(pos);
        const int rank = GetRankFromPos(pos);
        const int piece = newBoard[rank * 8 + file];

        if (piece == 0)
            return;

        const int pieceColor = GetPieceColor(piece);

        const int dx[] = {1, -1, 0, 0};
        const int dy[] = {0, 0, 1, -1};

        for (int dir = 0; dir < 4; dir++)
        {
            int newFile = file + dx[dir];
            int newRank = rank + dy[dir];

            while (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8)
            {
                int square = newBoard[newRank * 8 + newFile];

                if (square)
                {
                    if (GetPieceColor(square) != pieceColor)
                    {
                        moves.push_back(GetPos(newFile, newRank));
                    }
                    break;
                }

                moves.push_back(GetPos(newFile, newRank));
                newFile += dx[dir];
                newRank += dy[dir];
            }
        }
    }

    void GetBishopMoves(vector<int> &moves, int pos, int newBoard[SQUARES_NUM])
    {

        const int file = GetFileFromPos(pos);
        const int rank = GetRankFromPos(pos);
        const int piece = newBoard[rank * 8 + file];

        if (piece == 0)
            return;

        const int pieceColor = GetPieceColor(piece);

        const int dx[] = {1, -1, 1, -1};
        const int dy[] = {1, 1, -1, -1};

        for (int dir = 0; dir < 4; dir++)
        {
            int newFile = file + dx[dir];
            int newRank = rank + dy[dir];

            while (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8)
            {
                int square = newBoard[newRank * 8 + newFile];

                if (square)
                {
                    if (GetPieceColor(square) != pieceColor)
                    {
                        moves.push_back(GetPos(newFile, newRank));
                    }
                    break;
                }

                moves.push_back(GetPos(newFile, newRank));
                newFile += dx[dir];
                newRank += dy[dir];
            }
        }
    }

    void GetQueenMoves(vector<int> &moves, int pos, int newBoard[SQUARES_NUM])
    {
        GetRookMoves(moves, pos, newBoard);
        GetBishopMoves(moves, pos, newBoard);
    }

    void GetKnightMoves(vector<int> &moves, int pos, int newBoard[SQUARES_NUM])
    {
        const int file = GetFileFromPos(pos);
        const int rank = GetRankFromPos(pos);
        const int piece = newBoard[rank * 8 + file];

        if (piece == 0)
            return;

        const int pieceColor = GetPieceColor(piece);

        const int dx[] = {1, -1, 1, -1, 2, 2, -2, -2};
        const int dy[] = {2, 2, -2, -2, 1, -1, 1, -1};

        for (int dir = 0; dir < 8; dir++)
        {
            int newFile = file + dx[dir];
            int newRank = rank + dy[dir];

            if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8)
            {
                int square = newBoard[newRank * 8 + newFile];

                if (square == 0 || GetPieceColor(square) != pieceColor)
                {
                    moves.push_back(GetPos(newFile, newRank));
                }
            }
        }
    }

    void GetKingMoves(vector<int> &moves, int pos, int newBoard[SQUARES_NUM], bool castleKingSide, bool castleQueenSide)
    {
        const int file = GetFileFromPos(pos);
        const int rank = GetRankFromPos(pos);
        const int piece = newBoard[rank * 8 + file];

        if (piece == 0)
            return;

        const int pieceColor = GetPieceColor(piece);

        const int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
        const int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};

        for (int dir = 0; dir < 8; dir++)
        {
            int newFile = file + dx[dir];
            int newRank = rank + dy[dir];

            if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8)
            {
                int square = newBoard[newRank * 8 + newFile];

                if (square == 0 || GetPieceColor(square) != pieceColor)
                {
                    moves.push_back(GetPos(newFile, newRank));
                }
            }
        }

        if (castleKingSide)
        {
            if (newBoard[rank * 8 + 5] == 0 && newBoard[rank * 8 + 6] == 0)
            {
                moves.push_back(GetPos(6, rank));
            }
        }

        if (castleQueenSide)
        {
            if (newBoard[rank * 8 + 3] == 0 && newBoard[rank * 8 + 2] == 0 && board[rank * 8 + 1] == 0)
            {
                moves.push_back(GetPos(2, rank));
            }
        }
    }

    void GetPawnMoves(vector<int> &moves, int pos, int newBoard[SQUARES_NUM], int enPassantSquare)
    {
        const int file = GetFileFromPos(pos);
        const int rank = GetRankFromPos(pos);
        const int piece = newBoard[rank * 8 + file];

        if (piece == 0)
            return;

        const int pieceColor = GetPieceColor(piece);
        const int multiplier = pieceColor ? 1 : 0;

        const int dx[] = {0, 0, 1, -1};
        const int dy[] = {2, 1, 1, 1};
        if (pieceColor == 0)
        {
            const int dy[] = {-2, -1, 1, 1};
        }

        // double push

        if (newBoard[(rank + 1 * multiplier) * 8 + file] == 0 && newBoard[(rank + 2 * multiplier) * 8 + file] == 0)
        {
            moves.push_back(GetPos(file, rank + 2 * multiplier));
            moves.push_back(GetPos(file, rank + 1 * multiplier));
        }
        else if (newBoard[(rank + 1 * multiplier) * 8 + file] == 0)
        {
            moves.push_back(GetPos(file, rank + 1 * multiplier));
        }
    }

    bool inCheck(int newBoard[64], int ignore)
    {
        if (ignore) newBoard = board;
        for (int x = 0; x < SQUARES_NUM; x++)
        {
            int position = GetPosFromIdx(x);
            int piece = newBoard[x];
            if (piece != 0 && GetPieceColor(piece) != Turn)
            {
                vector<int> moves;
                GetLegalMoves(moves, position, newBoard, ignore);
                int kingPos = Turn ? KingPosW : KingPosB;
                if (find(moves.begin(), moves.end(), kingPos) != moves.end())
                {
                    return true;
                }
            }
        }
        return false;
    }

    int makeMove(int from, int to)
    {


        int fromFile = GetFileFromPos(from);
        int fromRank = GetRankFromPos(from);
        int toFile = GetFileFromPos(to);
        int toRank = GetRankFromPos(to);

        int square = board[fromRank * 8 + fromFile];
        int pieceColor = GetPieceColor(square);

        if (square == 0 || pieceColor%2!=Turn%2)
            return -1;

        int movedPieceType = GetPieceType(square);
        vector<int> moves;
        GetLegalMoves(moves, GetPos(fromFile, fromRank), {}, 1);

        for (int move : moves) {
            int newBoard[SQUARES_NUM];
            copy(begin(board), end(board), begin(newBoard));

            int moveFile = GetFileFromPos(move);
            int moveRank = GetRankFromPos(move);

            newBoard[moveRank * 8 + moveFile] = newBoard[fromRank * 8 + fromFile];
            newBoard[fromRank * 8 + fromFile] = 0;


            int res = inCheck(newBoard, 0);
            if (res) return -1;
        }


        
            board[toRank * 8 + toFile] = board[fromRank * 8 + fromFile];
            board[fromRank * 8 + fromFile] = 0;
        

        return 0;
    }

    void GetLegalMoves(vector<int> &moves, int pos, int newBoard[64], int ignore)
    {
        if (ignore) newBoard=board;

        int piece = newBoard[GetRankFromPos(pos) * 8 + GetFileFromPos(pos)];
        int pieceType = GetPieceType(piece);
        int pieceColor = GetPieceColor(piece);

        switch (pieceType)
        {
        case 1:
            GetKingMoves(moves, pos, newBoard, pieceColor ? KingSideCastleW : KingSideCastleB, pieceColor ? QueenSideCastleW : QueenSideCastleB);
            break;

        case 2:
            GetPawnMoves(moves, pos, newBoard, -1);
            break;

        case 3:
            GetKnightMoves(moves, pos, newBoard);
            break;

        case 4:
            GetBishopMoves(moves, pos, newBoard);
            break;

        case 5:
            GetRookMoves(moves, pos, newBoard);
            break;

        case 6:
            GetQueenMoves(moves, pos, newBoard);
            break;
        default:
            break;
        }
    }

    int GetPieceType(int value)
    {
        return (int)(value / 10);
    }

    int GetPieceColor(int value)
    {
        return (value % 10);
    }

    int GetFileFromPos(int pos)
    {
        return (int)(pos / 10);
    }
    int GetRankFromPos(int pos)
    {
        return pos % 10;
    }
    int GetPos(int file, int rank)
    {
        return 10 * file + rank;
    }
    int GetPosFromIdx(int idx)
    {
        return (idx % 8) * 10 + (idx - idx % 8) / 8;
    };
};

int convertNotation(str move) {
        std::map<char, int>
        notationMap = {
            {'a', 0},
            {'b', 1},
            {'c', 2},
            {'d', 3},
            {'e', 4},
            {'f', 5},
            {'g', 6},
            {'h', 7}};

        int fromFile = notationMap[move[0]];
        int fromRank = (int)move[1]-48-1;

        int toFile = notationMap[move[2]];
        int toRank = (int)move[3]-48-1;

        return fromFile*1000+fromRank*100+toFile*10+toRank;
}

int main()
{

    str CheckFen = "r1bqk1nr/pppp1ppp/2n5/4p3/1b1PP3/5N2/PPP2PPP/RNBQKB1R w KQkq - 1 4";
    str fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPP/RNBQKBNR w KQkq - 1 4";

    Board board(fen);
    board.DisplayBoard(())
    int result = board.makeMove(21, 22);

    if (result==-1) {


        cout << "Invalid Move" << endl;
    } else {
        cout << "Move Made" << endl;
        board.DisplayBoard({}, 1);
    }

    return 0;
}