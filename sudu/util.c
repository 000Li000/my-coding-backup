#include "util.h"

// 檢查盤面是否已完成（每格都填滿且正確）
extern int answer_board[9][9];
int is_complete(int board[][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) return 0;
            if (board[i][j] != answer_board[i][j]) return 0;
        }
    }
    return 1;
}

// 計算盤面剩餘空格數
int count_empty_cells(int board[][9]) {
    int count = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (board[i][j] == 0) count++;
    return count;
}
