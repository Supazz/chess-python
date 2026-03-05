#include "chess-library-master/chess-library-master/include/chess.hpp"
using namespace chess;
using namespace std;

constexpr int PAWN_VALUE = 1;
constexpr int KNIGHT_VALUE = 3;
constexpr int BISHOP_VALUE = 3;
constexpr int ROOK_VALUE = 5;
constexpr int QUEEN_VALUE = 9;
constexpr int INF = 1'000'000'000;
constexpr int MATE = 900'000'000;

struct Evaluation
{
    int eval;
    Move bestMove;
};

int evaluate(Board &board)
{
    if (board.isGameOver().first == GameResultReason::CHECKMATE)
    {
        // Its the losers turn when its checkmate
        if (board.sideToMove() == Color::WHITE)
        {
            return -MATE;
        }
        else
        {
            return MATE;
        }
    }
    else if (board.isGameOver().second == GameResult::DRAW)
    {
        return 0;
    }

    int evaluation = 0;

    Bitboard whitePawn = board.pieces(PieceType::PAWN, Color::WHITE);
    Bitboard whiteKnight = board.pieces(PieceType::KNIGHT, Color::WHITE);
    Bitboard whiteBishop = board.pieces(PieceType::BISHOP, Color::WHITE);
    Bitboard whiteRook = board.pieces(PieceType::ROOK, Color::WHITE);
    Bitboard whiteQueen = board.pieces(PieceType::QUEEN, Color::WHITE);
    Bitboard blackPawn = board.pieces(PieceType::PAWN, Color::BLACK);
    Bitboard blackKnight = board.pieces(PieceType::KNIGHT, Color::BLACK);
    Bitboard blackBishop = board.pieces(PieceType::BISHOP, Color::BLACK);
    Bitboard blackRook = board.pieces(PieceType::ROOK, Color::BLACK);
    Bitboard blackQueen = board.pieces(PieceType::QUEEN, Color::BLACK);

    int whiteCount = (whitePawn.count() * PAWN_VALUE) + (whiteKnight.count() * KNIGHT_VALUE) + (whiteBishop.count() * BISHOP_VALUE) + (whiteRook.count() * ROOK_VALUE) + (whiteQueen.count() * QUEEN_VALUE);
    int blackCount = (blackPawn.count() * PAWN_VALUE) + (blackKnight.count() * KNIGHT_VALUE) + (blackBishop.count() * BISHOP_VALUE) + (blackRook.count() * ROOK_VALUE) + (blackQueen.count() * QUEEN_VALUE);

    return whiteCount - blackCount;
}

Evaluation minMax(Board &board, int depth, int alpha, int beta)
{
    Evaluation evaluation;

    Movelist moves;
    movegen::legalmoves(moves, board);

    // Sort Moves, putting attacking moves at front
    sort(moves.begin(), moves.end(), [&board](const Move &a, const Move &b)
         {
                  bool aCapture = board.isCapture(a);
                  bool bCapture = board.isCapture(b);
                  return aCapture && !bCapture; });

    // If depth is 0, or game is over the use return evaluate board as the evaluation
    if (depth == 0 || moves.empty())
    {
        // depth 0 or terminal position: just evaluate the board
        evaluation.eval = evaluate(board);
        return evaluation;
    }

    if (board.sideToMove() == Color::WHITE)
    {
        evaluation.eval = -INF;

        for (const auto &move : moves)
        {
            board.makeMove<true>(move);
            int eval = minMax(board, depth - 1, alpha, beta).eval;
            board.unmakeMove(move);
            if (eval > evaluation.eval)
            {
                evaluation.bestMove = move;
                evaluation.eval = eval;
            }

            alpha = max(alpha, eval);

            if (beta <= alpha)
            {
                break;
            }
        }
    }

    if (board.sideToMove() == Color::BLACK)
    {
        evaluation.eval = INF;

        for (const auto &move : moves)
        {

            board.makeMove<true>(move);

            int eval = minMax(board, depth - 1, alpha, beta).eval;
            board.unmakeMove(move);
            if (eval < evaluation.eval)
            {
                evaluation.bestMove = move;
                evaluation.eval = eval;
            }

            beta = min(beta, eval);

            if (beta <= alpha)
            {
                break;
            }
        }
    }

    return evaluation;
}

int main(int argc, char *argv[])
{
    Board board(argv[1]);
    int depth = std::atoi(argv[2]);
    Evaluation evaluation = minMax(board, depth, -INF, INF);
    cout << uci::moveToUci(evaluation.bestMove, false) << endl;
    cout << evaluation.eval << endl;

    // Board board("8/8/8/8/8/1k4p1/1r6/K7 w - - 2 58");
    // cout << (board.isGameOver().second == GameResult::DRAW) << endl;

    // Movelist moves;
    // movegen::legalmoves(moves, board);

    // for (const auto &move : moves)
    // {
    //     std::cout << uci::moveToUci(move) << std::endl;
    // }
    return 0;
}