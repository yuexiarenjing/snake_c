#include "snake.h"

// 定义全局的WINDOWS变量
WINDOW * g_mainwin;

// 设置分数，游戏区域的宽度和高度
int g_score = 0, g_width, g_height;

// 食物的位置
pos fruit;

// 使用一维数组来表示游戏界面的所有二维位置坐标
bool *spaces;

// 定义队首和队尾指针
node *front=NULL, *back=NULL;

// 返回蛇头的位置，如果蛇头指针为NULL则返回NULL
pos* peek()
{
    return front == NULL ? NULL : front->position;
}

// 获得队列头的位置同时弹出第一个元素
pos* dequeue()
{
    // 临时位置指针
    pos *tmp = front->position;
    
    // 临时变量记录当前队列头
    node *oldfront = front;
    
    // 将front指针指向第二个节点
    front = front->next;
    
    // 将节点销毁
    free(oldfront);
    
    // 返回先前队列头的位置
    return tmp;
}

// 在队列尾插入新的节点
void enqueue(pos position)
{
    // 为新节点的位置结构和新节点分配空间
    pos *newpos   = (pos*)  malloc( sizeof( position ) ); 
    node *newnode = (node*) malloc( sizeof( node ) );

    // 初始化新节点的位置
    newpos->x = position.x;
    newpos->y = position.y;
    newnode->position = newpos;

    // 判断是否队列为空，如果为空则该节点为第一个节点
    if( front == NULL && back == NULL )
        front = back = newnode;
    else
    {
        // 如果不为空将该节点插入到队尾
        back->next = newnode;
        newnode->prev = back;
        
        // 重新设置队尾指针指向新插入的节点
        back = newnode;
    }
}


// 在游戏面板指定位置显示字符str
void snake_write_text( int y, int x, char* str )
{
    // mvwaddstr在窗口g_mainwin指定位置(x,y)输出字符串str
    mvwaddstr( g_mainwin, y , x, str );
}

// 显示游戏面板
void snake_draw_board( )
{
    int i;
    
    // 打印游戏面板边界，限定游戏区域
    for( i=0; i<g_height; i++ )
    {
        // 游戏面板边界显示为XXXXXX
        snake_write_text( i, 0,         "X" );
        snake_write_text( i, g_width-1, "X" );
    }
    for( i=1; i<g_width-1; i++ )
    {
        snake_write_text( 0, i,          "X" );
        snake_write_text( g_height-1, i, "X" );
    }
    
    // 在游戏面板下方显示分数
    snake_write_text( g_height+1, 2, "Score:" );
}

// 游戏结束，清理内存
void snake_game_over( )
{
    // 释放位置数组
    free( spaces );
    
    // 遍历队列，释放队列内的所有节点
    while( front )
    {
        node *n = front;
        front = front->next;
        free( n->position );
        free( n );
    }
    
    // 对应ncurses的initscr，结束ncurses窗口
    endwin();
    
    // 退出系统
    exit(0);
}

// 判断当前位置是否在游戏区域内
bool snake_in_bounds( pos position )
{
    return position.y < g_height - 1 && position.y > 0 && position.x < g_width - 1 && position.x > 0;
}

// 使用一维数组实现二维矩阵
// 所以需要将二维的位置坐标转换成一维数组的索引
int snake_cooridinate_to_index( pos position )
{
    return g_width * position.y + position.x;
}

// 使用一维数组实现二维矩阵
// 所以需要将一维数组的索引转换成二维的位置坐标
pos snake_index_to_coordinate( int index )
{
    int x = index % g_width;
    int y = index / g_width;
    return (pos) { x, y };
}

// 在游戏面板上随机产生食物
void snake_draw_fruit( )
{
    attrset( COLOR_PAIR( 3 ) );
    int idx;
    do
    {
        // 获得随机的索引值
        // 索引值不应超过数组的索引最大值：g_width * g_height
        idx = rand( ) % ( g_width * g_height );
        
        // 根据一维数组的索引，获得食物的位置坐标
        fruit = snake_index_to_coordinate( idx );
    }
    while( spaces[idx] || !snake_in_bounds( fruit ) );
    
    // 在游戏面板显示食物，使用字母F表示
    snake_write_text( fruit.y, fruit.x, "F" );
}

// 遍历移动蛇的所有节点
bool snake_move_player( pos head )
{
    attrset( COLOR_PAIR( 1 ) ) ;
    
    // 将位置坐标转换成位置数组索引
    int idx = snake_cooridinate_to_index( head );
    
    // 判断该位置是否已有节点
    // 如果已有节点表示蛇撞到墙或自己了，则游戏结束
    if( spaces[idx] )
        snake_game_over( );
    
    // 标志该位置被占用
    spaces[idx] = true;
    
    // 将该位置插入队列
    enqueue( head );
    
    // 队列移动一个节点，分数增加十分
    g_score += 10;
    
    // 判断该位置是否为食物
    if( head.x == fruit.x && head.y == fruit.y )
    {
        // 如果为食物则需要产生新的食物
        snake_draw_fruit( );
        
        // 分数增加1000分
        g_score += 1000;
    }
    else
    {
        // 如果不是食物，则蛇身体没有增长
        // 向前移动了一步，需要把尾部节点执行出队操作
        pos *tail = dequeue( );
        
        // 将该位置置为空闲
        spaces[snake_cooridinate_to_index( *tail )] = false;
        
        // 该位置显示为空
        snake_write_text( tail->y, tail->x, " " );
    }
    
    // 在新位置显示S，表示蛇身体的一部分
    snake_write_text( head.y, head.x, "S" );
    
    // 更新记分板
    char buffer[25];
    
    // 更新记分板内容，设置显示的配色
    sprintf( buffer, "%d", g_score );
    attrset( COLOR_PAIR( 2 ) );
    snake_write_text( g_height+1, 9, buffer );

}

// 主函数
int main( int argc, char *argv[] )
{

    // 设置默认按键
    int key = KEY_RIGHT;
    
    // 初始化屏幕
    if( ( g_mainwin = initscr() ) == NULL ) {
        
        // 如果ncurses初始化屏幕失败则打印出错信息退出
        perror( "error initialising ncurses" );
        exit( EXIT_FAILURE );
    }
    
    // 设置随机函数
    srand( time( NULL ) );
    
    // 关闭回显模式
    // ncurses回显模式用来决定用户输入是否立刻在光标处显示
    noecho( );
    
    // 设置显示可视效果，2表示为very visible 
    curs_set( 2 );
    
    // 设置半延时模式，1指定半延时的刷新时间间隔为100毫秒
    // 这个数字可以控制蛇行动的速度
    halfdelay( 10 );
    
    // 打开Keypad模式：打开对逻辑键的支持
    keypad( g_mainwin, TRUE );
    
    // 对颜色显示进行初始化
    start_color( );
    
    // 定义颜色组合
    init_pair( 1, COLOR_RED,     COLOR_BLACK );
    init_pair( 2, COLOR_GREEN,   COLOR_BLACK );
    init_pair( 3, COLOR_YELLOW,  COLOR_BLACK );
    init_pair( 4, COLOR_BLUE,    COLOR_BLACK );
    init_pair( 5, COLOR_CYAN,    COLOR_BLACK );
    init_pair( 6, COLOR_MAGENTA, COLOR_BLACK );
    init_pair( 7, COLOR_WHITE,   COLOR_BLACK );
    
    // 返回g_mainwin窗口中最大的行数并存储在g_height变量中
    // 返回最大的列数并存储在g_width变量中
    getmaxyx( g_mainwin, g_height, g_width );
    
    // 获得真实的屏幕高度和宽度
    g_width  = g_width  < WINDOWS_WIDTH  ? g_width  : WINDOWS_WIDTH;
    g_height = g_height < WINDOWS_HEIGHT ? g_height : WINDOWS_HEIGHT; 
    
    // 根据屏幕宽度和高度为数组分配内存地址
    spaces = (bool*) malloc( sizeof( bool ) * g_height * g_width );

    // 显示snake游戏中的面板
    snake_draw_board( );
    
    // 显示snake游戏中的食物
    snake_draw_fruit( );
    
    // 设置第一个节点位置
    pos head = { 5,5 };
    enqueue( head );
    
    // 程序主循环
    while( 1 )
    {
        // 获得键盘输入        
        int in = getch( );
        
        // 判断输入是否为错误
        if( in != ERR )
            key = in;
        
        // 根据键盘输入进行判断
        switch( key )
        {
            // 如果输入为方向键下，k或K
            // 则头节点向下位移一位
            case KEY_DOWN:
            case 'k':
            case 'K':
                head.y++;
                break;
            
            // 如果输入为方向键右，l或L
            // 则头节点向右移动一位
            case KEY_RIGHT:
            case 'l':
            case 'L':
                head.x++;
                break;
                
            // 如果输入为方向键上，j或J
            // 则头节点向上移动一位
            case KEY_UP:
            case 'j':
            case 'J':
                head.y--;
                break;
                
            // 如果输入为方向键左，h或H
            // 则头节点向左移动一位
            case KEY_LEFT:
            case 'h':
            case 'H':
                head.x--;
                break;

        }
        
        // 判断头节点是否撞到边界
        // 如果装到则游戏结束，否则移动整条蛇
        if( !snake_in_bounds( head ) )
            snake_game_over( );
        else
            snake_move_player( head );
    }
    
    // 循环退出游戏结束
    snake_game_over( );
}

