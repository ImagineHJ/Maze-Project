#pragma once

#include "ofMain.h"
#include <string.h>

typedef struct _cell{
    int w, h;
    struct _cell *left;
    struct _cell *right;
    struct _cell *up;
    struct _cell *down;
    struct _cell *parent;    /* save parent cell */
    bool visited;            /* visited flag in DFS, BFS */
}cell;

typedef struct _wall{
    bool right;
    bool down;
}wall;

typedef struct _node{
    struct _wall *wall;
    struct _node *link;
}node;

typedef struct _stnode{
    cell *cell;
    struct _stnode *link;
}stnode;

typedef struct _path{
    int w1, h1, w2, h2;
    struct _path *link;
}path;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    // flag variables
    int load_flag;
    int dfs_flag;
    int bfs_flag;
    
    // maze info for draw
    node *wall_head;
    int width;
    int height;
    
    // maze represented in graph
    cell ***cell_ptr;
    cell *maze_start;
    cell *maze_end;
    
    // linked list to save answer path
    path *answer_path;

    bool readFile(const char* fileName);
    void makeTree(void);
    void freeMemory(void);
    
    /* save the answer path info in linked list */
    void savePath(path **, path **, cell *, cell *);
    /* delete the answer path from memory and initialize visited flag */
    void deletePath(void);
    
    // stack operations
    void push(stnode **, cell *);
    cell *pop(stnode **);
    cell *top(stnode *);
    
    //queue operations
    void enque(stnode **, stnode **, cell *);
    cell *deque(stnode **, stnode **);
    
};
