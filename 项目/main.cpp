#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<string>
#include<map>
#include<assert.h>
#include<iostream>
#include<cstdlib>
using namespace std;
using namespace sf;

const int M = 20;//下落区高度
const int N = 10;//下落区宽度

struct Point //坐标
{
    int x, y;
}a[4], b[4], c[4], d[4];  //a为当前坐标，b为备份，c为下一个,d为着陆点
class Game {
public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game();
    bool load();//加载资源
    void run();//运行
    //初始参数
    static int sco;//初始分数
    static int round;//初始波数
    const int Ez = 10;//波数/难度
    const int Mid = 20;
    const int Hd = 30;
    float DELAY = 0.3;
    int G = 0;//开挂,0关1开
    const  float QUICK = 0.05;//按down加速速度
    const float INTERVAL = 0.07;//不同难度下落时间间隔
    int field[M][N] = { 0 };//下落区，初始为0，1为有方块
    int new_field[4][4] = { 0 };//下一个方块展示区
private:
    //功能函数与变量
    void menu();//菜单
    void processEvents();//输入事件
    void update();//更新帧
    void render();//渲染帧
    void newblock();//新方块
    void newnewblock();//预览方块
    bool check(Point*);//检查是否合法,适用所有方块坐标
    bool lose();//是否结束
    void gameover();//结束界面
    void coming_spot();//方块下落位置
    void turn();//旋转
    void fall();//下落
    void clear();//消除
    void restart();//重置参数
    RenderWindow window;//窗口
    int start = 0;//菜单界面判断是否开始游戏
    int dx = 0;//左右位移
    bool rotate = 0;//旋转
    int colorNum = 1;//砖块颜色
    int newcolorNum = 1;//下一砖块颜色
    int flag_ac = 0;//加速标记
    float timer = 0, delay = DELAY;//计时器，下落间隔
    void acclerate();//加速，根据波数确定速度
    //资源变量
    Clock clock;//计时
    Sound sou_1, sou_2, sou_3, sou_4, sou_down, sou_gg;//音效
    Music menu_m, game_m;//音乐
    SoundBuffer smash_1, smash_2, smash_3, smash_4, ac, gg;//内存音效
    Font font;//字体
    Text score;//分数
    Text ro;//波数
    Texture t, t_ls, t_menu, t_bg, t_go;//纹理---t:普通砖块,t_ls:虚化落地显示砖块,t_menu：菜单背景
    //t_bg:游戏背景,t_go:gameover背景
    Sprite s, ns, ls, mn, bg, go;//精灵------s:当前砖块，ns:新砖块，ls:着陆砖块，mn:菜单,bg:游戏背景，go:gameover
    int figures[7][4] =  ///砖块形状
    {
        1,3,5,7,//1形
        2,4,5,7,//S形
        3,5,4,6,//之形
        3,5,4,7,//凸形
        2,3,5,7,//L形
        3,5,7,6,//J形
        2,3,4,5//正方形
        //////////////////////
        /* ---------方块形状
         | 0 | 1 |
         | 2 | 3 |
         | 4 | 5 |
         | 6 | 7 |
         ---------*/
         /////////////////
    };
};
Game::Game() :window(VideoMode(500, 800), "Tetris") {

}
int Game::sco = 0;
int Game::round = 0;
bool Game::load() {

    if (!game_m.openFromFile("audio/CJMG.ogg"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }

    if (!menu_m.openFromFile("audio/quiet.ogg"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }
    if (!font.loadFromFile("fonts/msyh.ttc"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }

    if (!smash_1.loadFromFile("audio/手抖的厉害.wav"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }
    if (!smash_2.loadFromFile("audio/试图把我激怒.wav"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }
    if (!smash_3.loadFromFile("audio/歪比巴卜.ogg"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }
    if (!smash_4.loadFromFile("audio/太哈人了.ogg"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }
    if (!ac.loadFromFile("audio/可不敢乱快进.ogg"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }
    if (!gg.loadFromFile("audio/啊？就这.wav"))
    {
        cout << "error" << endl;
        system("pause");
        return 0;
    }
    sou_1.setBuffer(smash_1);
    sou_2.setBuffer(smash_2);
    sou_3.setBuffer(smash_3);
    sou_4.setBuffer(smash_4);
    sou_down.setBuffer(ac);
    sou_gg.setBuffer(gg);

    t.loadFromFile("image/tiles35.png");
    t_ls.loadFromFile("image/tiles35light.png");
    t_menu.loadFromFile("image/menu.png");
    t_bg.loadFromFile("image/bg1.png");
    t_go.loadFromFile("image/gg.png");
    s.setTexture(t);
    ns.setTexture(t);
    ls.setTexture(t_ls);
    mn.setTexture(t_menu);
    bg.setTexture(t_bg);
    go.setTexture(t_go);
    s.setTextureRect(IntRect(0, 0, 35, 35));//IntRect(x,y,宽，高)
    ns.setTextureRect(IntRect(0, 0, 35, 35));
    ls.setTextureRect(IntRect(0, 0, 35, 35));

    go.setPosition(78, 313);

    score.setFont(font);
    score.setString("0");
    score.setFillColor(Color(229, 140, 133));
    score.setPosition(24, 62);

    ro.setFont(font);
    ro.setString("0");
    ro.setFillColor(Color(229, 140, 133));
    ro.setPosition(24, 160);
    return true;
}

void Game::restart() {
    delay = DELAY;
    G = 0;
    start = 0;
    sco = 0;
    score.setString("0");
    round = 0;
    ro.setString("0");
    score.setPosition(24, 62);//初始化分数显示位置
    memset(field, 0, sizeof(field));//初始化下落区
}

void Game::menu() {
    menu_m.play();//播放菜单音乐
    while (window.isOpen() && start == 0)
    {

        Event event;
        while (window.pollEvent(event) && start == 0)
        {

            if (event.type == Event::Closed)//关闭游戏
                window.close();
            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Num1) { DELAY = 0.3; start = 1; menu_m.stop(); }
                else if (event.key.code == Keyboard::Num2) { DELAY = 0.2; start = 1; menu_m.stop(); }
                else if (event.key.code == Keyboard::Num3) { G = 1; start = 1; menu_m.stop(); }
                else if (event.key.code == Keyboard::Escape) { menu_m.stop(); window.close(); }

        }
        window.clear();
        window.draw(mn);
        window.display();
    }
}

void Game::turn() {
    if (rotate && colorNum != 7)//colorNum==7为正方形砖块
    {
        Point p = a[1];//旋转中心
        for (int i = 0; i < 4; i++)
        {
            int x = a[i].y - p.y;
            int y = a[i].x - p.x;
            a[i].x = p.x - x;
            a[i].y = p.y + y;

        }
        if (!check(a))
            for (int i = 0; i < 4; i++)
            {
                a[i] = b[i];
            }
    }
}

void Game::fall() {
    if (timer > delay)
    {
        for (int i = 0; i < 4; i++)
        {
            b[i] = a[i];
            a[i].y += 1;

        }
        if (check(a) == false)//落地
        {
            for (int i = 0; i < 4; i++)
            {
                field[b[i].y][b[i].x] = colorNum;
            }
            round++;
            ro.setString(to_string(round));
            newnewblock();//左侧显示未下落的砖块
            newblock();//新的下落砖块
            coming_spot();//当前砖块落地形状
        }
        //落地点
        else
            for (int i = 0; i < 4; i++)
                d[i] = a[i];
        while (check(d) != false)
            for (int i = 0; i < 4; i++)
            {
                d[i].y += 1;
            }

        timer = 0;
    }
}

void Game::clear() {
    int k = M - 1;
    int combo = 0;
    for (int i = M - 1; i > 0; i--)
    {
        int count = 0;
        for (int j = 0; j < N; j++)
        {
            if (field[i][j])
                count++;
            field[k][j] = field[i][j];
        }
        if (count < N) k--;
        else
        {
            combo++;//多行消除连击分数
        }

    }
    switch (combo)//不同连击不同音效
    {
    case 1:sou_1.play(); break;
    case 2:sou_2.play(); break;
    case 3:sou_3.play(); break;
    case 4:sou_4.play(); break;
    }
    sco += combo * combo;
    score.setString(to_string(sco));
}

bool Game::check(Point* a)
{
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return 0;
        else if (field[a[i].y][a[i].x]) return 0;//超出边界
    return 1;
}

bool Game::lose()
{
    for (int i = 0; i < N; i++)
        if (field[1][i]) return 1;//超出边界
    return 0;
}

void Game::coming_spot() {
    int n = colorNum - 1;
    for (int i = 0; i < 4; i++)
    {
        d[i].x = figures[n][i] % 2;
        d[i].y = figures[n][i] / 2;
    }
}

void Game::newnewblock() {
    if (round > 0)
        colorNum = newcolorNum;//上一个预览砖块颜色

    newcolorNum = 1 + rand() % 7;

    int n = newcolorNum - 1;
    //把序号转换成坐标值：序号%2得到x坐标，序号/2得到y坐标（转换方法与坐标怎么定的有关）
    if (G)
        n = 0;
    for (int i = 0; i < 4; i++)
    {
        c[i].x = figures[n][i] % 2;
        c[i].y = figures[n][i] / 2;
    }
}

void Game::newblock() {

    //开挂,注释掉下面三行，解注释第四行--------->7.22已实现菜单选择开挂模式，可修改第355行n改变开挂模式下落形状
    if (round == 0)
        colorNum = 1 + rand() % 7;
    int n = colorNum - 1;
    //int n = 0;
    if (G)//开挂模式,n确定某一形状
        n = 0;
    //把序号转换成坐标值：序号%2得到x坐标，序号/2得到y坐标（转换方法与坐标怎么定的有关）
    for (int i = 0; i < 4; i++)
    {
        a[i].x = figures[n][i] % 2;
        a[i].y = figures[n][i] / 2;
    }

}

void Game::acclerate() {
    if (round < Ez)
        delay = DELAY;
    else if (round < Mid)
        delay = DELAY - INTERVAL;
    else if (round < Hd)
        delay = DELAY - 2 * INTERVAL;
}

void Game::gameover() {

    Event event;
    sou_gg.play();
    score.setPosition(230, 410);//显示在结束面板
    score.setFillColor(Color(229, 140, 133));
    while (start == 1)//运行过menu()
    {
        while (window.pollEvent(event) && start == 1) {
            if (event.type == sf::Event::Closed)
                window.close();
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
            {
                start = 0;
            }
        }

        window.draw(go);
        window.draw(score);
        window.display();

    }
    game_m.stop();

}
void Game::run() {
    while (start == 0 && window.isOpen()) {
        restart();
        menu();
        game_m.setLoop(true);
        game_m.play();
        newnewblock();
        newblock();
        while (window.isOpen()) {

            processEvents();
            if (start == 0)break;
            update();
            if (lose())
            {
                gameover();
                game_m.stop();
                break;

            }
            render();
        }
        game_m.stop();
    }
}

void Game::processEvents() {
    float time = clock.getElapsedTime().asSeconds();
    clock.restart();
    timer += time;
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)//关闭游戏
            window.close();

        if (event.type == Event::KeyPressed)
            if (event.key.code == Keyboard::Up) rotate = true;
            else if (event.key.code == Keyboard::Left) dx = -1;
            else if (event.key.code == Keyboard::Right) dx = 1;
            else if (event.key.code == Keyboard::Escape)
            {
                gameover();
                game_m.stop();

            }
    }

    if (Keyboard::isKeyPressed(Keyboard::Down))
    {
        delay = QUICK;//加速下落
        flag_ac++;
        if (Event::EventType::KeyReleased)//加速音效
        {
            if (event.key.code == sf::Keyboard::Key::Down)
            {

                if ((flag_ac / 250.0) > 1)
                    sou_down.play();
                flag_ac = 0;
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        b[i] = a[i];
        a[i].x += dx;
    }



}
void Game::update() {
    /// <- 移动 -> ///

    if (!check(a))//如果越界，则在原位
        for (int i = 0; i < 4; i++)
        {
            a[i] = b[i];
        }

    /// <- 旋转 -> ///
    turn();

    for (int i = 0; i < 4; i++)//不加的话显示的落地点的砖块不能和正在下落的砖块水平同步
    {
        d[i].x = a[i].x;
    }

    ///自动下落///
    fall();

    ///消除///
    clear();

    dx = 0; rotate = 0;
    acclerate();//根据波数确定速度
}
void Game::render() {
    ///draw///
    window.clear(Color::White);

    window.draw(bg);

    //已落底积累的方块
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
        {
            if (field[i][j] == 0)continue;
            s.setTextureRect(IntRect(field[i][j] * 35, 0, 35, 35));
            s.setPosition(j * 35, i * 35);
            s.move(140, 49);//offset
            window.draw(s);
        }
    for (int i = 0; i < 4; i++)
    {
        s.setTextureRect(IntRect(colorNum * 35, 0, 35, 35));//当前方块
        s.setPosition(a[i].x * 35, a[i].y * 35);//方格定位
        s.move(140, 49);//下落位置修正
        window.draw(s);
    }
    //当前沉底位置
    for (int i = 0; i < 4; i++)
    {
        ls.setTextureRect(IntRect(colorNum * 35, 0, 35, 35));//当前方块
        ls.setPosition(d[i].x * 35, d[i].y * 35);//方格定位
        ls.move(140, 14);//下落位置修正
        window.draw(ls);
    }
    //下一个方块
    for (int i = 0; i < 4; i++)
    {
        ns.setTextureRect(IntRect(newcolorNum * 35, 0, 35, 35));
        ns.setPosition(c[i].x * 35, c[i].y * 35);//方格定位
        ns.move(20, 250);//下落位置修正，屏幕左侧
        window.draw(ns);
    }

    window.draw(score);//显示分数

    window.draw(ro);//显示波数

    window.display();
}

int main() {
    Game game;
    if (game.load())
    {
        game.run();
        sleep(seconds(1));
    }
    else return -1;
    return 0;
}
