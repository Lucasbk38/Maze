#include "EasyBMP.hpp"
#include "HSLToRGB.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <fstream>


using namespace std;
using namespace EasyBMP;

const int GOOD_PATH = -3;
const int TAIL = -2;
const int WALL = -1;
const int EMPTY = 0;
const int COLOR_COEF = 4;

RGBColor allColor (const int &v) {
    return RGBColor(v, v, v);
}

const RGBColor TAIL_COLOR = allColor(255);
const RGBColor WALL_COLOR = allColor(0);
const RGBColor EMPTY_COLOR = allColor(255);
const RGBColor GOOD_PATH_COLOR = RGBColor(0, 255, 0);

struct Point {
    int x, y;

    Point (const int &x, const int &y) : x(x), y(y) {}
};

class Maze
{
    private:
        int _width, _height, _pixelSize, _s, _rs, _n;
        vector< vector<int> > _matrix;
        Image _img;

    public:
        Maze(const int &width, const int &height, const int &pixelSize);

        void save (const string &path);
        void draw (const int &x, const int &y);
        void drawAll ();
        void saveState (const int &logRate, const bool &showProgressBar);

        void init ();
        void generate (const int &logRate, ofstream &o);
        void resolvePath (const int &logRate);
        void hideAll (const int &logRate);
};



Maze::Maze(const int &width, const int &height, const int &pixelSize) : _width(width * 2 + 1), _height(height * 2 + 1), _pixelSize(pixelSize), _img(_width * pixelSize, _height * pixelSize), _matrix(_height, vector<int>(_width, WALL)), _s(0), _rs(0), _n(width * height * 2) {}

void Maze::save (const string &path) {
    _img.Write(path);
}

void Maze::draw (const int &x, const int &y) {
    const int v = _matrix[y][x];

    RGBColor rgb =
        v == TAIL ? TAIL_COLOR :
        v == EMPTY ? EMPTY_COLOR :
        v == WALL ? WALL_COLOR :
        v == GOOD_PATH ? GOOD_PATH_COLOR :
        HSVtoRGB((v / COLOR_COEF) % 361, 100, 100);

    int bx = x * _pixelSize;
    int by = y * _pixelSize;

    for (int dy = 0; dy < _pixelSize; dy++)
    for (int dx = 0; dx < _pixelSize; dx++)
    _img.SetPixel(bx + dx, by + dy, rgb);
}

void Maze::drawAll () {
    for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width ; x++)
    draw(x, y);
}

void logProgressBar (const int &_s, const int &_n) {
    double d =(double)_s / (double)_n * 100;
    double r = d - floor(d);

    int n = d;
    string s;

        if (r < 0.25)
        s = "░";
    else if (r < 0.50)
        s = "▒";
    else if (r < 0.75)
        s = "▓";
    else
        s = "█";

    cout << '\r' << _s << " / " << _n << " : " << n << "% [";
    for (int i = 0; i < n; i++)
        cout << "█";
    cout << s;
    for (int i = n + 1; i < 100; i++)
        cout << ' ';
    cout << ']' << flush;
}

void Maze::saveState (const int &logRate, const bool &showProgressBar) {
    if (logRate == 0) {
        if (showProgressBar)
            if (_s % 10000 == 0)
                logProgressBar(_s, _n);
    } else if (_s % logRate == 0) {
        stringstream ss;
        ss << "img/img";
        ss << _rs;
        ss << ".bmp";

        save(ss.str());

        if (showProgressBar)
            logProgressBar(_s, _n);
        else
            cout << _s << " -> " << _rs << endl;

        _rs++;
    }

    _s++;
}

void Maze::init () {
    drawAll();

    for (int y = 1; y < _height; y += 2)
    for (int x = 1; x < _width ; x += 2)
        _matrix[y][x] = EMPTY;
    
    // drawAll();
}

void Maze::generate (const int &logRate, ofstream &o) {
    saveState(true, true);

    vector<Point> deltas;
    deltas.push_back(Point(0,  1));
    deltas.push_back(Point(0, -1));
    deltas.push_back(Point( 1, 0));
    deltas.push_back(Point(-1, 0));

    vector<Point> initArr;
    initArr.push_back(Point(1, 1));
    initArr.push_back(Point(1, 1));

    vector< vector<Point> > stack; // tile, wall
    stack.push_back(initArr);
    _matrix[1][1] = TAIL;
    draw(1, 1);
    _matrix[1][1] = 1;

    int i = 1;
    int n = 1;

    o << '[';

    do {
        const vector<Point> stackEl = stack[n - 1];
        const Point e = stackEl[0];
        vector<Point> possibilities;

        for (int i = 0; i < 4; i++)
        {
            const Point delta = deltas[i];
            const int x = e.x + delta.x * 2;
            const int y = e.y + delta.y * 2;

            if (0 < x && x < _width )
            if (0 < y && y < _height)
            if (_matrix[y][x] == EMPTY)
                possibilities.push_back(delta);
        }

        const int s = possibilities.size();
        vector<Point> arr;

        if (s != 0) {
            Point d(-1, -1);
            if (s == 0) d = possibilities[0];
            else d = possibilities[rand() % s];


            const Point w = Point(e.x + d.x,     e.y + d.y    );
            const Point p = Point(e.x + d.x * 2, e.y + d.y * 2);

            _matrix[w.y][w.x] = TAIL;
            _matrix[p.y][p.x] = TAIL;

            draw(w.x, w.y);
            draw(p.x, p.y);

            saveState(logRate, true);

            _matrix[w.y][w.x] = 2 * n;
            _matrix[p.y][p.x] = 2 * n + 1;

            arr.push_back(p);
            arr.push_back(w);
            stack.push_back(arr);
        } else {
            Point tile = stackEl[0];
            Point wall = stackEl[1];

            draw(tile.x, tile.y);
            draw(wall.x, wall.y);

            saveState(logRate, true);

            stack.pop_back();
        }


        n = stack.size();
        i++;

        o << n;
        if (n != 0) o << ',';
    } while (n > 0);

    o << ']';

    saveState(true, true);
}

void Maze::resolvePath (const int &logRate) {
    vector<Point> deltas;
    deltas.push_back(Point(-1, 0));
    deltas.push_back(Point(0, -1));
    deltas.push_back(Point( 1, 0));
    deltas.push_back(Point(0,  1));

    Point p(_width - 2, _height - 2);

    while (!(p.x == 1 && p.y == 1)) {
        int v = _matrix[p.y][p.x];
        _matrix[p.y][p.x] = GOOD_PATH;
        draw(p.x, p.y);

        saveState(logRate, false);

        for (int di = 0; di < 4; di++) {
            Point d = deltas[di];

            const int x = p.x + d.x;
            const int y = p.y + d.y;

            if (_matrix[y][x] == v - 1) {
                p = Point(x, y);
                break;
            }
        }
    }

    _matrix[1][1] = GOOD_PATH;
    draw(1, 1);
}

void Maze::hideAll (const int &logRate) {
    if (logRate == 0) {
        for (int y = 0; y < _height; y++)
        for (int x = 0; x < _width; x++)
        if (0 < x && x < _width )
        if (0 < y && y < _height) {
            int v = _matrix[y][x];

            
            if (v != WALL && v != GOOD_PATH) {
                _matrix[y][x] = EMPTY;
                draw(x, y);
            }
        }
        
        
    } else {
        int n = _width + _height;

        for (int diag = 0; diag < n; diag++) {
            for (int e = 0; e < diag; e++)
            {
                int x = diag - e;
                int y = e;
                if (0 < x && x < _width )
                if (0 < y && y < _height) {
                    int v = _matrix[y][x];

                    
                    if (v != WALL && v != GOOD_PATH) {
                        _matrix[y][x] = EMPTY;
                        draw(x, y);
                    }
                }
            }

            saveState(logRate, false);
        }
    }

    saveState(1, false);
}



int main () {
    ofstream o;
    o.open("data.json");

    const int generateLogRate = 10;
    const int solveLogRate = 10;
    const int hideLogRate = 10;
    
    
    srand(13042011 + 7);
    Maze maze(64, 36, 1);

    maze.init();
    maze.generate(generateLogRate, o);// 10 -> 205; 25 -> 486; 50 -> 1185; 100 -> 2332
    maze.drawAll();

    maze.save("img.bmp");

    cout << endl << "Solving :" << endl;
    maze.resolvePath(solveLogRate);

    cout << endl << "Hiding :" << endl;
    maze.hideAll(hideLogRate);

    maze.save("solved.bmp");

    o.close();

    return 0;
}
