#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "board.h"
#include "util.h"
#include "rlutil.h" // 請確保 rlutil.h 已複製到本目錄或正確 include

// ========== 題庫：多組數獨題目 ==========
// 修改題庫內容（範例：題目1第一格改為5，題目2第一格改為3，題目3第一格改為7）
#define PROBLEM_COUNT 3
int problems[PROBLEM_COUNT][9][9] = {
    { // 題目1
        {5, 0, 0, 0, 0, 0, 0, 9, 0},
        {1, 9, 0, 4, 7, 0, 6, 0, 8},
        {0, 5, 2, 8, 1, 9, 4, 0, 7},
        {2, 0, 0, 0, 4, 8, 0, 0, 0},
        {0, 0, 9, 0, 0, 0, 5, 0, 0},
        {0, 0, 0, 7, 5, 0, 0, 0, 9},
        {9, 0, 7, 3, 6, 4, 1, 8, 0},
        {5, 0, 6, 0, 8, 1, 0, 7, 4},
        {0, 8, 0, 0, 0, 0, 0, 0, 0}
    },
    { // 題目2
        {3, 2, 0, 6, 0, 8, 0, 0, 0},
        {5, 8, 0, 0, 0, 9, 7, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 0},
        {3, 7, 0, 0, 0, 0, 5, 0, 0},
        {6, 0, 0, 0, 0, 0, 0, 0, 4},
        {0, 0, 8, 0, 0, 0, 0, 1, 3},
        {0, 0, 0, 0, 2, 0, 0, 0, 0},
        {0, 0, 9, 8, 0, 0, 0, 3, 6},
        {0, 0, 0, 3, 0, 6, 0, 9, 0}
    },
    { // 題目3
        {7, 0, 0, 0, 0, 0, 2, 0, 0},
        {0, 8, 0, 0, 0, 7, 0, 9, 0},
        {6, 0, 2, 0, 0, 0, 5, 0, 0},
        {0, 7, 0, 0, 6, 0, 0, 0, 0},
        {0, 0, 0, 9, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 2, 0, 0, 4, 0},
        {0, 0, 5, 0, 0, 0, 6, 0, 3},
        {0, 9, 0, 4, 0, 0, 0, 7, 0},
        {0, 0, 6, 0, 0, 0, 0, 0, 0}
    }
};

// 數獨盤面，0 表示空格，1-9 為已填入的數字
int board[9][9] = {
    {0, 0, 0, 0, 0, 0, 0, 9, 0},
    {1, 9, 0, 4, 7, 0, 6, 0, 8},
    {0, 5, 2, 8, 1, 9, 4, 0, 7},
    {2, 0, 0, 0, 4, 8, 0, 0, 0},
    {0, 0, 9, 0, 0, 0, 5, 0, 0},
    {0, 0, 0, 7, 5, 0, 0, 0, 9},
    {9, 0, 7, 3, 6, 4, 1, 8, 0},
    {5, 0, 6, 0, 8, 1, 0, 7, 4},
    {0, 8, 0, 0, 0, 0, 0, 0, 0}
};

// ========== 遊戲用全域變數 ==========
// player_board：玩家目前填寫的盤面
// answer_board：正確答案盤面（由遞迴解題產生）
// original_board：原始題目盤面（不可更動的格子）
// error_count：玩家答錯次數
int player_board[9][9];
int answer_board[9][9];
int original_board[9][9];
int error_count = 0;

// 儲存盤面到文字檔案
// 將盤面以純文字格式（0 轉為 .）儲存，每行9個字元
void save_to_text_file(int board[][9], const char* filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("無法開啟檔案 %s 進行寫入\n", filename);
        return;
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) fprintf(fp, ".");
            else fprintf(fp, "%d", board[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("已成功儲存到 %s\n", filename);
}

// 從文字檔案讀取盤面
// 讀取純文字檔案，將 . 轉為 0，數字字元轉為對應整數
int read_from_text_file(int board[][9], const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("無法開啟檔案 %s 進行讀取\n", filename);
        return 0;
    }
    char line[20];
    int row = 0;
    while (row < 9 && fgets(line, sizeof(line), fp) != NULL) {
        for (int col = 0; col < 9; col++) {
            if (line[col] == '.') board[row][col] = 0;
            else if (line[col] >= '1' && line[col] <= '9')
                board[row][col] = line[col] - '0';
        }
        row++;
    }
    fclose(fp);
    if (row < 9) {
        printf("警告：檔案格式不正確或檔案不完整\n");
        return 0;
    }
    printf("已成功從 %s 讀取數獨盤面\n", filename);
    return 1;
}

// 二進位檔案結構
// SudokuDataHeader：檔案標頭，記錄問題數量與每筆資料大小
// SudokuProblem：單一數獨問題（含編號與盤面資料）
typedef struct {
    int numbers;
    int datasize;
} SudokuDataHeader;

typedef struct {
    int id;
    int data[9][9];
} SudokuProblem;

// 儲存盤面到二進位檔案
// 可選擇覆寫或附加，支援多個問題存於同一檔案
void save_to_binary_file(int board[][9], int problem_id, const char* filename, int is_append) {
    FILE *fp;
    if (is_append) {
        fp = fopen(filename, "rb+");
        if (fp == NULL) {
            fp = fopen(filename, "wb+");
            if (fp == NULL) {
                printf("無法建立檔案 %s\n", filename);
                return;
            }
            SudokuDataHeader header;
            header.numbers = 1;
            header.datasize = sizeof(SudokuProblem);
            fwrite(&header, sizeof(header), 1, fp);
        } else {
            SudokuDataHeader header;
            fread(&header, sizeof(header), 1, fp);
            header.numbers++;
            fseek(fp, 0, SEEK_SET);
            fwrite(&header, sizeof(header), 1, fp);
            fseek(fp, 0, SEEK_END);
        }
    } else {
        fp = fopen(filename, "wb");
        if (fp == NULL) {
            printf("無法開啟檔案 %s 進行寫入\n", filename);
            return;
        }
        SudokuDataHeader header;
        header.numbers = 1;
        header.datasize = sizeof(SudokuProblem);
        fwrite(&header, sizeof(header), 1, fp);
    }
    SudokuProblem problem;
    problem.id = problem_id;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            problem.data[i][j] = board[i][j];
    fwrite(&problem, sizeof(problem), 1, fp);
    fclose(fp);
    printf("已成功儲存到二進位檔案 %s\n", filename);
}

// 從二進位檔案讀取盤面
// 讀取指定編號的數獨問題，載入到指定盤面
int read_from_binary_file(int board[][9], const char* filename, int problem_index) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("無法開啟檔案 %s 進行讀取\n", filename);
        return 0;
    }
    SudokuDataHeader header;
    fread(&header, sizeof(header), 1, fp);
    printf("檔案中有 %d 個數獨問題\n", header.numbers);
    if (problem_index < 0 || problem_index >= header.numbers) {
        printf("問題編號 %d 超出範圍 (0-%d)\n", problem_index, header.numbers - 1);
        fclose(fp);
        return 0;
    }
    fseek(fp, sizeof(header) + problem_index * header.datasize, SEEK_SET);
    SudokuProblem problem;
    fread(&problem, sizeof(problem), 1, fp);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            board[i][j] = problem.data[i][j];
    printf("已讀取問題 ID: %d\n", problem.id);
    fclose(fp);
    return 1;
}

// 檢查某數字能否填入指定位置
// 檢查行、列、3x3 區塊是否有重複
int isValid(int number, int puzzle[][9], int row, int col) {
    int rowStart = (row / 3) * 3;
    int colStart = (col / 3) * 3;
    for (int i = 0; i < 9; i++) {
        if (puzzle[row][i] == number) return 0;
        if (puzzle[i][col] == number) return 0;
        if (puzzle[rowStart + (i / 3)][colStart + (i % 3)] == number) return 0;
    }
    return 1;
}

// 遞迴解題主函式
// pos 為目前處理的位置（0~80），成功回傳 1，否則回傳 0
int solve(int puzzle[][9], int pos) {
    if (pos == 81) return 1;
    int row = pos / 9, col = pos % 9;
    if (puzzle[row][col] != 0) return solve(puzzle, pos + 1);
    for (int num = 1; num <= 9; num++) {
        if (isValid(num, puzzle, row, col)) {
            puzzle[row][col] = num;
            if (solve(puzzle, pos + 1)) return 1;
            puzzle[row][col] = 0;
        }
    }
    return 0;
}

// ========== 遊戲初始化函式 ==========
// 複製原始盤面，產生答案盤面，重設錯誤次數
void init_game(int problem_index) {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            board[i][j] = problems[problem_index][i][j];
            original_board[i][j] = board[i][j];
            player_board[i][j] = board[i][j];
            answer_board[i][j] = board[i][j];
        }
    solve(answer_board, 0);
    error_count = 0;
    printf("遊戲初始化完成！\n");
}

// ========== 輸入處理函式 ==========
// 處理玩家輸入，檢查合法性與正確性，回傳狀態
int handle_input() {
    int row, col, num;
    char input[100];
    int first = 1;
    while (1) {
        if (first) {
            printf("\n請輸入 [行] [列] [數字] (1-9)，或輸入 0 0 0 結束遊戲：\n> ");
            first = 0;
        } else {
            printf("> ");
        }
        if (!fgets(input, sizeof(input), stdin)) {
            printf("輸入錯誤，請重新輸入。\n");
            continue;
        }
        // 若讀到只有換行或空白，直接重新輸入，不顯示錯誤
        int only_space = 1;
        for (char *p = input; *p; ++p) {
            if (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') {
                only_space = 0;
                break;
            }
        }
        if (only_space) continue;
        // 解析輸入
        if (sscanf(input, "%d %d %d", &row, &col, &num) != 3) {
            printf("格式錯誤！請輸入三個數字（例如：3 5 7）。\n");
            continue;
        }
        if (row == 0 && col == 0 && num == 0) return -1;
        if ((row < 1 || row > 9 || col < 1 || col > 9 || num < 1 || num > 9)) {
            // 允許 0 0 0 結束，其餘超出範圍才提示
            printf("輸入超出範圍！請輸入 1-9 之間的數字。\n");
            continue;
        }
        row--; col--;
        if (original_board[row][col] != 0) {
            printf("該位置是原始數字，不能修改！\n");
            return 0;
        }
        if (player_board[row][col] != 0) {
            printf("該位置已經填過數字了！\n");
            return 0;
        }
        if (answer_board[row][col] == num) {
            player_board[row][col] = num;
            printf("正確！\n");
            return 1;
        } else {
            error_count++;
            printf("錯誤！錯誤次數：%d\n", error_count);
            return 1;
        }
    }
}

// ========== 題目選擇 ==========
int select_problem() {
    int choice;
    printf("請選擇題號 (1~%d)，或輸入 0 隨機選題：", PROBLEM_COUNT);
    scanf("%d", &choice);
    if (choice == 0) {
        srand((unsigned int)time(NULL));
        int rnd = rand() % PROBLEM_COUNT;
        printf("隨機選擇第 %d 題\n", rnd + 1);
        return rnd;
    }
    if (choice < 1 || choice > PROBLEM_COUNT) {
        printf("輸入錯誤，自動選擇第 1 題\n");
        return 0;
    }
    return choice - 1;
}

// ========== 新增：目前游標位置 ==========
int cursor_row = 0, cursor_col = 0;

// ========== 修改：盤面顯示加上顏色與游標 ==========
void print_board_with_color(int board[][9], int cursor_row, int cursor_col) {
    cls();
    printf("\n +-------+-------+-------+\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0) printf(" | ");
            else printf(" ");
            if (i == cursor_row && j == cursor_col) {
                setColor(LIGHTRED); // 游標位置特別顏色
                if (board[i][j] == 0) printf("_");
                else printf("%d", board[i][j]);
                resetColor();
            } else if (original_board[i][j] != 0) {
                setColor(LIGHTBLUE); // 題目原始數字改為淺藍色
                printf("%d", board[i][j]);
                resetColor();
            } else if (board[i][j] == 0) {
                setColor(GREY); // 空格
                printf("_");
                resetColor();
            } else {
                setColor(GREEN); // 玩家填入改為綠色
                printf("%d", board[i][j]);
                resetColor();
            }
        }
        printf(" |\n");
        if (i % 3 == 2) printf(" +-------+-------+-------+\n");
    }
}

// ========== 修改：鍵盤互動填數 ==========
int handle_input_arrow() {
    int num = 0;
    while (1) {
        print_board_with_color(player_board, cursor_row, cursor_col);
        printf("\n使用方向鍵移動，1-9填數，0清除，q離開\n");
        printf("目前游標：(%d,%d)\n", cursor_row+1, cursor_col+1);
        int ch = getkey();
        if (ch == KEY_UP) {
            if (cursor_row > 0) cursor_row--;
        } else if (ch == KEY_DOWN) {
            if (cursor_row < 8) cursor_row++;
        } else if (ch == KEY_LEFT) {
            if (cursor_col > 0) cursor_col--;
        } else if (ch == KEY_RIGHT) {
            if (cursor_col < 8) cursor_col++;
        } else if (ch >= '1' && ch <= '9') {
            num = ch - '0';
            if (original_board[cursor_row][cursor_col] != 0) {
                printf("該位置是原始數字，不能修改！\n");
                msleep(600);
                continue;
            }
            if (answer_board[cursor_row][cursor_col] == num) {
                player_board[cursor_row][cursor_col] = num;
                printf("正確！\n");
            } else {
                error_count++;
                printf("錯誤！錯誤次數：%d\n", error_count);
                if (error_count == 2) {
                    // 給予提示：找出第一個尚未填寫或填錯的格子，並依照正確答案給提示
                    for (int i = 0; i < 9; i++) {
                        for (int j = 0; j < 9; j++) {
                            if (player_board[i][j] != answer_board[i][j]) {
                                printf("提示：第 %d 行第 %d 列的答案是 %d\n", i+1, j+1, answer_board[i][j]);
                                msleep(2000); // 停留2秒
                                i = 9; // 跳出外層迴圈
                                break;
                            }
                        }
                    }
                }
            }
            msleep(600);
        } else if (ch == '0') {
            if (original_board[cursor_row][cursor_col] == 0)
                player_board[cursor_row][cursor_col] = 0;
        } else if (ch == 'q' || ch == 'Q') {
            printf("遊戲結束！\n");
            return -1;
        } else if (ch == 'a' || ch == 'A') {
            // 新增：查看完整答案
            printf("\n=== 數獨答案 ===\n");
            print_board_with_color(answer_board, -1, -1);
            printf("(按任意鍵繼續)\n");
            getkey();
        }
        if (error_count >= 5) return -1;
        if (is_complete(player_board)) return 2;
    }
}

// 假設你的 is_complete 需要一個 int 二維陣列和大小
int is_complete(int board[9][9]) {
    // 範例：檢查是否有 0，若有則未完成
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[i][j] == 0) return 0;
        }
    }
    return 1;
}

// ========== 修改：遊戲主函式 ==========
void play_game() {
    printf("=== 數獨遊戲 ===\n");
    printf("規則：方向鍵移動，1-9填數，0清除，q離開\n");
    int problem_index = select_problem();
    init_game(problem_index);
    cursor_row = 0; cursor_col = 0;
    while (1) {
        int result = handle_input_arrow();
        if (result == -1) {
            break;
        }
        if (result == 2) {
            print_board_with_color(player_board, cursor_row, cursor_col);
            printf("🎉 恭喜！你完成了數獨！\n");
            break;
        }
        if (error_count >= 5) {
            print_board_with_color(player_board, cursor_row, cursor_col);
            printf("💥 錯誤太多次，遊戲結束！\n");
            printf("正確答案：\n");
            print_board_with_color(answer_board, -1, -1);
            break;
        }
    }
}

// ========== 修改：外部終端機顏色與游標支援 ==========
int main() {
    // 若在 Windows 命令提示字元(cmd)執行，建議加上這行讓顏色正確顯示
    system("chcp 65001 > nul");
    // system("cls"); // 清除螢幕（可選）

    play_game();
    return 0;
}
