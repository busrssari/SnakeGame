#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UP 'w'
#define DOWN 's'
#define RIGHT 'd'
#define LEFT 'a'

typedef struct board {
  char type;
  int value;
} block;

typedef struct point {
  int row;
  int col;
} point;

block ***init_board(void);
void play(block ***);
void draw_board(block ***, point *, int);
int move(block ***, point *, int *, char *, int *, int *);
void update(block ***, point *, int *, int, int, int *, int *);
int check_status(block ***, point *, int);

int main() {
  srand(time(NULL));
  block ***board;
  board = init_board();
  play(board);

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      free(board[i][j]);
    }
    free(board[i]);
  }
  free(board);
  return 0;
}



block ***init_board(void) { // 3 boyutlu board olusturulur
  block ***board;
  board = (block ***)malloc(10 * sizeof(int **));
  int i, j,bait_row, bait_col, obstacle_row, obstacle_col;
  for (i = 0; i < 10; i++) {
    board[i] = (block **)malloc(10 * sizeof(int *));
    for (j = 0; j < 10; j++) {
      board[i][j] = (block *)malloc(10 * sizeof(int));
    }
  }

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      board[i][j]->type = 'e'; 
      board[i][j][0].value = 0;
    }
  }

  do {    
    bait_row = rand() % 8 + 1; 
    bait_col = rand() % 8 + 1;
    obstacle_row = rand() % 8 + 1;
    obstacle_col = rand() % 8 + 1;
  } while ((bait_row == 1 && bait_col == 1) || (obstacle_row == 1 && obstacle_col == 1) || (bait_row==obstacle_row && bait_col==obstacle_col));  // ilk yem ve ilk engel
  board[bait_row][bait_col]->type = 'b';
  board[bait_row][bait_col]->value = 0;
  board[obstacle_row][obstacle_col][0].type = 'o';
  board[obstacle_row][obstacle_col][0].value= 1; //olusturulan engelin size ının ilk degerine 1 verilir
  return board;
}



void draw_board(block ***board, point *snake, int snakeSize)
{
    int size = 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {

            if (i == 0 || i == 9)   printf("- ");

            else if (j == 0 || j == 9)   printf("| ");

            else if (board[i][j]->type == 'b')   printf(". ");

            else if (board[i][j]->type == 'o') {

                if (board[i][j][0].value != 0)   printf("%d ", board[i][j]->value);   // obstacles icin
            
                else if (i == snake[0].row && j == snake[0].col)   printf("O ");
            
                else   printf("X ");

            }

             else  printf("  ");
        }
    printf("\n");
}
}



void play(block ***board) {
    point *snake;
    int snakeLength = 1, num_of_moves = 0, num_of_obstacles = 1;
    snake = (point *)calloc(snakeLength, sizeof(point)); // snakein ilk halini oluşturup yerleştiriyoruz
    snake[0].row = 1;
    snake[0].col = 1;
    board[snake[0].row][snake[0].col]->type = 'o';
    board[snake[0].row][snake[0].col]->value = 0;
    char previous_move = 'X';

    printf("\nWelcome to the game!\n\n");
    do {
        draw_board(board, snake, snakeLength);
    } while (move(board, snake, &snakeLength, &previous_move, &num_of_moves, &num_of_obstacles) == 0);
    printf("\nGame Over...\n\n");
    free(snake);
}



int check_status(block ***board, point *snake, int length) { 

  if (snake[0].row == 0 || snake[0].col == 0 || snake[0].row == 9 || snake[0].col == 9) { // eger duvara carparsa
    return 1;
  }
  if (board[snake[0].row][snake[0].col]->type == 'o') {
    if (length < board[snake[0].row][snake[0].col]->value) { // eger kendinden buyuk bir engele carparsa
      return 1;
    }
  for (int i = 1; i < length; i++) { //eger yılan kendine carparsa
      if (snake[0].row == snake[i].row && snake[0].col == snake[i].col) {
        return 1;
      }
    }
  }
  return 0;
}



void update(block ***board, point *snake, int *length, int prevHeadsRow, int prevHeadsCol, int *num_of_moves, int *num_of_obstacles) {
  int flag = 0;
  if (board[snake[0].row][snake[0].col]->type == 'b') {  // eyer yılanın basının yeni yerinde bait varsa
    (*length)++;
    snake = realloc(snake, *length * sizeof(point)); // yılan boyutu arttırılır
    flag = 1;
  }

if (board[snake[0].row][snake[0].col]->type == 'o'){    // ...check fonk da kendinden buyuk engel gelmesi durumunu kontrol etmistik 
    (*num_of_obstacles)--;     // yılan bir engelle karsılasırsa yutar 
    board[snake[0].row][snake[0].col]->value = 0;
  }

  if (*length == 1) {    //burada yılanın sonu için tip donusumu yapıyoruz
    board[prevHeadsRow][prevHeadsCol]->type = 'e'; 
  }
  else {
    board[snake[*length - 1].row][snake[*length - 1].col]->type = 'e';
  }
  board[snake[0].row][snake[0].col]->type = 'o'; 

  for (int i = *length - 1; i > 0; i--) {   // yılanın vucudunu kaydırıyoruz
    if (i == 1) {
      snake[i].row = prevHeadsRow;  // baştan sonraki blok basın eski yerine kayar
      snake[i].col = prevHeadsCol;
    }
    else {
      snake[i].row = snake[i - 1].row;
      snake[i].col = snake[i - 1].col;
    }
  }


  if (flag == 1) {   // eger yılan uzadıysa 
    int bait_row, bait_col;
    do {    // yeni yemin koordinatlarını belirliyoruz
      bait_row = rand() % 8 + 1; 
      bait_col = rand() % 8 + 1;
    } while (board[bait_row][bait_col]->type != 'e');
    board[bait_row][bait_col]->type = 'b';
  }

  if (*num_of_moves == 5) {   // bes hamlede bir engelleri guncelliyoruz
    flag = 0;
    int obstacle_row, obstacle_col;
    do {
      flag = 0;
      obstacle_row = rand() % 8 + 1;
      obstacle_col = rand() % 8 + 1;

      for (int i = 0; i < *length; i++) {
        if (obstacle_row == snake[i].row && obstacle_col == snake[i].col) {   // eger yem yılanın uzerine geldiyse flag=1
          flag = 1;
          break;
        }
      }

      if (board[obstacle_row][obstacle_col]->value == 9) {   // eger yeni koordinatlar max boyuttaki bir engele denk geliyorsa .. 
        flag = 1; 
      }

      if (flag == 0) {
        if ((*num_of_obstacles) >= 3) {   // eger max obstacle sayısına ulastıysak var olanın uzerine ekleme yapıyoruz
          if (board[obstacle_row][obstacle_col]->type != 'o') {
            flag = 1;
          }
        }
      }
    } while (flag == 1);

    if (board[obstacle_row][obstacle_col]->type == 'e') {
      (*num_of_obstacles)++;
    }
    board[obstacle_row][obstacle_col]->type = 'o';
    board[obstacle_row][obstacle_col]->value++;
    (*num_of_moves) = 0;   // hamle sayısını sıfırlıyoruz
  }
}



int move(block ***board, point *snake, int *length, char *previous_move, int *num_of_moves, int *num_of_obstacles) {
    char move,waste;
    printf("Please enter your move(w,a,s,d): ");
    scanf(" %c",&move);
    scanf("%c",&waste);   // capraz hamleleri engellemek için

    int prevHeadsRow = snake[0].row;  // yılanın başının eski koordinatlarını tutuyoruz
    int prevHeadsCol = snake[0].col;

    switch (move) {
    case UP:
        if (*previous_move != DOWN) {   // yılanın geldiği yone gitmesini engellemek için
            snake[0].row--;   // yılanın basını hareket ettiriyoruz
            if (check_status(board, snake, *length) == 1) {   // eger yaptığımız hamle oyunu bitirdiyse move 1 doner 
                return 1;
            }
            (*num_of_moves)++;
            update(board, snake, length, prevHeadsRow, prevHeadsCol, num_of_moves, num_of_obstacles);  //aksi halde update cagırılır
            *previous_move = move;
        }
        break;

    case DOWN:
        if (*previous_move != UP) {
            snake[0].row++;
            if (check_status(board, snake, *length) == 1)
            {
                return 1;
            }
            (*num_of_moves)++;
            update(board, snake, length, prevHeadsRow, prevHeadsCol, num_of_moves, num_of_obstacles);
            *previous_move = move;
        }
        break;

    case RIGHT:
        if (*previous_move != LEFT) {
            snake[0].col++;
            if (check_status(board, snake, *length) == 1) {
                return 1;
            }
            (*num_of_moves)++;
            update(board, snake, length, prevHeadsRow, prevHeadsCol, num_of_moves, num_of_obstacles);
            *previous_move = move;
        }
        break;

    case LEFT:
        if (*previous_move != RIGHT && check_status(board, snake, *length) == 0) {
            snake[0].col--;
            if (check_status(board, snake, *length) == 1) {
                return 1;
            }
            (*num_of_moves)++;
            update(board, snake, length, prevHeadsRow, prevHeadsCol, num_of_moves, num_of_obstacles);
            *previous_move = move;
        }
        break;

    default:
        printf("invalid move\n");
        break;
    }
    return 0;
}
