#include <stdio.h>

// 格式化輸出盤面
// 以美觀的方式印出 9x9 數獨盤面，空格用底線表示
void print_board(int board[][9]) {
    printf("\n +-------+-------+-------+\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0) printf(" | ");
            else printf(" ");
            if (board[i][j] == 0) printf("_");
            else printf("%d", board[i][j]);
        }
        printf(" |\n");
        if (i % 3 == 2) printf(" +-------+-------+-------+\n");
    }
}
