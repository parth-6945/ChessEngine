# Premove Chess Engine

Premove is a small, educational chess engine written in C. It follows the classic engine architecture used in many beginner-to-intermediate chess engine tutorials: board representation, move generation, move making/unmaking, perft testing, evaluation, alpha-beta search, and a basic transposition table.

This project is inspired by the Bluefever Software chess engine playlist:
https://www.youtube.com/playlist?list=PLZ1QII7yudbc-Ky058TEaOstZHVbT-2hg

## Features

- Bitboard-friendly board representation with a 10x12 internal board array
- Move generation for legal moves
- Make/unmake move handling
- Perft testing for move generation correctness
- Simple evaluation function
- Alpha-beta search with move ordering
- Transposition table for faster search
- Console mode, UCI mode, and XBoard mode support

## Project Structure

- `defs.h` – core types, constants, move encoding helpers, and engine declarations
- `board.c` – board initialization, FEN parsing, board validation, and board printing
- `attack.c` – attack detection and king safety checks
- `movegen.c` – legal move generation
- `makemove.c` – make/unmake logic
- `search.c` – alpha-beta search and quiescence search
- `evaluate.c` – static evaluation function
- `pvtable.c` – transposition table implementation
- `perft.c` – perft test utilities
- `uci.c` – UCI protocol support
- `xboard.c` – XBoard and console loop support
- `premove.c` – entry point for the engine
- `makefile` – build instructions

## Building

The project uses a simple Makefile.

```bash
cd /chess_engine
make
```

This builds the executable `premove`.

## Running the Engine

### UCI mode

```bash
./premove
```

Then type:

```text
uci
```

You can interact with the engine using standard UCI commands such as:
- `position startpos`
- `go depth 4`
- `quit`

## Suggested GUI

If you want to play against the engine visually, a good free GUI is Cutechess:
https://github.com/cutechess/cutechess

## Engine Options
- Book moves enable\disable
- Hash table size 64 MB to 1024 MB
- Threads used by engine 1 thread to 32 threads

## Notes

This is a learning-focused engine. It is not intended to be a top-tier competitive engine, but it is a solid example of how a chess engine is structured and how core techniques are implemented in C.

## Acknowledgements

- Bluefever Software for the excellent engine programming playlist
- The chess programming community for many of the ideas and techniques used here
