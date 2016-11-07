#ifndef __SNAKE_H_
#define __SNAKE_H_

// 引入相关库头文件
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <stdio.h>
#include <time.h>

// 引入ncurses库头文件
#include <curses.h>

// 定义游戏界面大小
#define WINDOWS_WIDTH  70
#define WINDOWS_HEIGHT 25

// 定义位置坐标结构体
typedef struct s_position {
    int x;
    int y;
} pos;


// 使用双向链表实现贪吃蛇的队列
typedef struct s_node
{
    pos *position;
    struct s_node *prev;
    struct s_node *next;
} node;

// 队列操作函数：

// 1. peek操作：获得队列头的位置
pos* peek();

// 2. dequeue操作：获得队列头的位置同时弹出第一个节点
pos* dequeue();

// 3. enqueue操作：在队列尾插入新的节点
void enqueue(pos position);

// 贪吃蛇相关函数：

// 1. 在游戏面板指定位置显示字符str
void snake_write_text(int y, int x, char* str);

// 2. 显示游戏面板
void snake_draw_board( );

// 3. 游戏结束，清理内存
void snake_game_over( );

// 4. 判断当前位置是否在游戏区域内
bool snake_in_bounds( pos position );

// 5. 将二维的位置坐标转换成一维数组的索引
int snake_cooridinate_to_index( pos position );

// 6. 将一维数组的索引转换成二维位置坐标
pos snake_index_to_coordinate( int index );

// 7. 在游戏面板上随机产生食物
void snake_draw_fruit( );

// 8. 遍历移动蛇的所有节点
bool snake_move_player( pos head );


#endif //__SNAKE_H_

