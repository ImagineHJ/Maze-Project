#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(15); // Limit the speed of our program to 15 frames per second
    
    // We still want to draw on a black background, so we need to draw
    // the background before we do anything with the brush
    ofBackground(255,255,255);
    ofSetLineWidth(4);

    load_flag = 0;
    dfs_flag = 0;
    bfs_flag = 0;
    width = 0;
    height = 0;
    answer_path = NULL;     // initialize path
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
   
    
    if( load_flag ){

        /* manage cell size and line size by the size of the maze */
        int CELL_SIZE=50, LINE_SIZE =10, X_STR=5, Y_STR=5;
        if(width>=15 || height>=15) {CELL_SIZE=30; LINE_SIZE =6;}
        if(width>=25 || height>=25) {CELL_SIZE=20; LINE_SIZE =4;}
        if(width>=50 || height>=50) {CELL_SIZE=10; LINE_SIZE =2;}
        if(width>=100 || height>=100) {CELL_SIZE=8; LINE_SIZE =1;}
        if(width>=150 || height>=150) {CELL_SIZE=4; LINE_SIZE =1;}
        ofSetLineWidth(LINE_SIZE);
        ofSetColor(ofColor::blue);
        
        node *ptr = wall_head;
        ofDrawLine(X_STR, Y_STR, X_STR+CELL_SIZE*width, Y_STR);
        for(int i=0; i<height; i++){
            ofDrawLine(X_STR, Y_STR+CELL_SIZE*i, X_STR, Y_STR+CELL_SIZE*(i+1));
            for(int j=0; j<width; j++){
                if(ptr->wall[j].right==true)
                    ofDrawLine(X_STR+CELL_SIZE*(j+1), Y_STR+CELL_SIZE*i, X_STR+CELL_SIZE*(j+1), Y_STR+CELL_SIZE*(i+1));
                 if(ptr->wall[j].down==true)
                    ofDrawLine(X_STR+CELL_SIZE*j, Y_STR+CELL_SIZE*(i+1), X_STR+CELL_SIZE*(j+1), Y_STR+CELL_SIZE*(i+1));
            }
            ptr=ptr->link;
        }
        
        if(dfs_flag || bfs_flag){
            /* draw visited path */
            ofSetColor(ofColor::gray);
            for(int j=0; j<height; j++)
                for(int i=0; i<width; i++){
                    if(cell_ptr[j][i]!=maze_start && cell_ptr[j][i]->visited)   /* check if the cell is visited */
                        ofDrawLine(X_STR+CELL_SIZE/2+CELL_SIZE*cell_ptr[j][i]->parent->w, Y_STR+CELL_SIZE/2+CELL_SIZE*cell_ptr[j][i]->parent->h, X_STR+CELL_SIZE/2+CELL_SIZE*cell_ptr[j][i]->w, Y_STR+CELL_SIZE/2+CELL_SIZE*cell_ptr[j][i]->h);
                }
            
            /* draw answer path */
            ofSetColor(ofColor::red);
            path *tmp = answer_path;
            while(tmp){
                ofDrawLine(X_STR+CELL_SIZE/2+CELL_SIZE*tmp->w1, Y_STR+CELL_SIZE/2+CELL_SIZE*tmp->h1, X_STR+CELL_SIZE/2+CELL_SIZE*tmp->w2, Y_STR+CELL_SIZE/2+CELL_SIZE*tmp->h2);
                tmp = tmp -> link;
            }
            /* mark start and end cell */
            ofSetColor(ofColor::lightGreen);
            ofDrawRectangle(X_STR+CELL_SIZE/2+CELL_SIZE*maze_start->w-LINE_SIZE/2, Y_STR+CELL_SIZE/2+CELL_SIZE*maze_start->h-LINE_SIZE/2, LINE_SIZE, LINE_SIZE);
            ofSetColor(ofColor::lightGreen);
            ofDrawRectangle(X_STR+CELL_SIZE/2+CELL_SIZE*maze_end->w-LINE_SIZE/2, Y_STR+CELL_SIZE/2+CELL_SIZE*maze_end->h-LINE_SIZE/2, LINE_SIZE, LINE_SIZE);
           }
    }
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'v') {
        // HACK: only needed on windows, when using ofSetAutoBackground(false)
        glReadBuffer(GL_FRONT);
        ofSaveScreen("savedScreenshot_"+ofGetTimestampString()+".png");
    }
    if (key == 'q'){
        
        load_flag = 0;
        dfs_flag = 0;
        bfs_flag = 0;
        
        freeMemory();
        cout << "Dynamically allocated memory has been freed." << endl;
        _Exit(0);
    }
    if (key == 'd'){
        if(!load_flag || dfs_flag ) return;
        if(bfs_flag){
            bfs_flag = 0;
            deletePath();
        }
        
        /* Finding maze path using iterative DFS */
        cell *ptr, *adj;
        path *ans_pre;
        stnode *stack=NULL;     //initialize stack
        
        maze_start->visited = true;
        push(&stack, maze_start);
        while(stack!=NULL){
            ptr = top(stack);
            /* find non-visited adj vertex */
            if( ptr->right && !(ptr->right->visited) )
                adj = ptr->right;
            else if( ptr->down && !(ptr->down->visited) )
                adj = ptr->down;
            else if( ptr->left && !(ptr->left->visited) )
                adj = ptr->left;
            else if( ptr->up && !(ptr->up->visited) )
                adj = ptr->up;
            else{           /* no more adj vertex to visit */
                pop(&stack);
                continue;
            }
            adj->parent = ptr;      /* save parent info */
            adj->visited = true;    /* make it visited */
            push(&stack, adj);      /* push in the stack */
            if(adj == maze_end) break;      /* found the answer path! */
        }
        
        ptr = pop(&stack);
        while(stack!=NULL){     /* save answer path in linked list */
            adj = pop(&stack);
            savePath(&answer_path, &ans_pre, ptr, adj);
            ptr = adj;
        }
        dfs_flag = 1;           /* draw the path on screen */
    }
    if (key == 'b'){
        
        if(!load_flag || bfs_flag ) return;
        if(dfs_flag){
            dfs_flag = 0;
            deletePath();
        }
        
        /* Finding maze path using BFS */
        cell *ptr, *tmp;
        path *ans_pre;
        stnode *queue=NULL, *queue_end = NULL;     //initialize queue
        
        maze_start->visited = true;
        enque(&queue, &queue_end, maze_start);
        
        while(queue!=NULL){
            ptr = deque(&queue, &queue_end);
            //cout<<ptr->h<<" "<<ptr->w<<endl;
            /* for all adj vertex */
            if( ptr->right && !(ptr->right->visited) ){
                ptr->right->visited = true;              /* make it visited */
                ptr->right->parent = ptr;                /* save parent info */
                if(ptr->right == maze_end ) break;       /* found the answer path! */
                enque(&queue, &queue_end, ptr->right);   /* push in the stack */
            }
            if( ptr->down && !(ptr->down->visited) ){
                ptr->down->visited = true;
                ptr->down->parent = ptr;
                if(ptr->down == maze_end ) break;
                enque(&queue, &queue_end, ptr->down);
            }
            if( ptr->left && !(ptr->left->visited) ){
                ptr->left->visited = true;
                ptr->left->parent = ptr;
                if(ptr->left == maze_end ) break;
                enque(&queue, &queue_end, ptr->left);
            }
            if( ptr->up && !(ptr->up->visited) ){
                ptr->up->visited = true;
                ptr->up->parent = ptr;
                if(ptr->up == maze_end ) break;
                enque(&queue, &queue_end, ptr->up);
            }
        }
        
        /* save answer path in linked list */
        ptr = maze_end;
        while(ptr!=maze_start){
            tmp = ptr->parent;      /* follow the parent */
            savePath(&answer_path, &ans_pre, tmp, ptr);
            ptr = tmp;
        }
        bfs_flag = 1;               /* draw the path on screen */
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if( key == 'l'){
        if(load_flag){
            load_flag = 0; dfs_flag = 0; bfs_flag = 0;
            width = 0; height = 0;
            answer_path = NULL;
            freeMemory();
        }
        // Open the Open File Dialog
        ofFileDialogResult openFileResult= ofSystemLoadDialog("Select a only txt for Maze");
        
        // Check whether the user opened a file
        if( openFileResult.bSuccess){
            ofLogVerbose("User selected a file");
            
            // We have a file, so let's check it and process it
            if(readFile(openFileResult.getName().c_str()))
                load_flag = 1;
            else return;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
 
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

bool ofApp::readFile(const char* fileName) {
    //Path to the comma delimited file
    //string fileName = "input.txt";
    
    ofFile file(fileName);
    
    if( !file.exists()) {
        cout << "Target file does not exists." << endl;
        return false;
    }
    else cout << "We found the target file." << endl;
    
    ofBuffer buffer(file);

    bool vert_flag = true;
    node *pre, *ptr;
    // Read file line by line
    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        height++;
        
        // first line
        if( height == 1){
            width = (line.length()-1)/2;
            continue;
        }
        
        if(vert_flag){
            ptr = (node *)malloc(sizeof(node));
            ptr->link = NULL;
            if(height == 2) wall_head = ptr;
            else pre->link = ptr;
            pre = ptr;
            ptr->wall = (wall *)malloc(sizeof(wall)*width);
            for(int i=2; i < width*2+1; i+=2){
                if(line[i]=='|') ptr->wall[i/2-1].right = true;
                else ptr->wall[i/2-1].right = false;
            }
            vert_flag = false;
        }
        else{
            for(int i=1; i < width*2+1; i+=2){
                if(line[i]=='-') ptr->wall[i/2].down = true;
                else ptr->wall[i/2].down = false;
            }
            vert_flag = true;
        }
        
    } // End of for-loop (Read file line by line).
    height = (height-1)/2;
    cout<<"width: "<<width<<" height: "<<height<<endl;
    
    makeTree();
    /*
    ptr=wall_head;
    for(int j=0; j<height; j++){
        for(int i=0; i<width; i++){
            if(ptr->wall[i].right == true) cout<<" |";
            else cout<<"  ";
        }
        cout<<endl;
        for(int i=0; i<width; i++){
            if(ptr->wall[i].down == true) cout<<"-+";
            else cout<<" +";
        }
        cout<<endl;
        ptr=ptr->link;
    }*/
    return true;
}

void ofApp::makeTree(){
    node *ptr=wall_head;
    
    cell_ptr = (cell ***)malloc(height*sizeof(cell**));
    for(int j=0; j<height; j++){
        cell_ptr[j]=(cell**)malloc(width*sizeof(cell*));
        for(int i=0; i<width; i++){
            cell_ptr[j][i]=(cell *)malloc(sizeof(cell));
            cell_ptr[j][i]->h = j; cell_ptr[j][i]->w = i;
            cell_ptr[j][i]->left = NULL; cell_ptr[j][i]->right = NULL;
            cell_ptr[j][i]->up = NULL; cell_ptr[j][i]->down = NULL;
            cell_ptr[j][i]->visited = false; cell_ptr[j][i]->parent = NULL;
        }
    }
    
    for(int j=0; j<height; j++){
        for(int i=0; i<width; i++){
            if(ptr->wall[i].right == false){
                cell_ptr[j][i]->right = cell_ptr[j][i+1];
                cell_ptr[j][i+1]->left = cell_ptr[j][i];
            }
            if(ptr->wall[i].down == false){
                cell_ptr[j][i]->down = cell_ptr[j+1][i];
                cell_ptr[j+1][i]->up = cell_ptr[j][i];
            }
        }
        ptr=ptr->link;
    }
    
    maze_start = cell_ptr[0][0];
    maze_end = cell_ptr[height-1][width-1];
    
}

void ofApp::freeMemory(){
    node *ptr=wall_head, *tmp;
    
    for(int j=0; j<height; j++){
        tmp = ptr;
        ptr = ptr->link;
        free(tmp->wall);
        free(tmp);
        for(int i=0; i<width; i++)
            free(cell_ptr[j][i]);
        free(cell_ptr[j]);
    }
    free(cell_ptr);
    
    if(bfs_flag || dfs_flag) deletePath();
}

/* stack operations */
void ofApp::push(stnode **stack, cell *item){
    stnode *ptr = (stnode *)malloc(sizeof(stnode *));
    
    ptr->cell = item;
    ptr->link = (*stack);
    (*stack) = ptr;
    
}
cell* ofApp::pop(stnode **stack){
    stnode *ptr = (*stack);
    cell *tmp = ptr->cell;
    (*stack)=(*stack)->link;
    free(ptr);
    return tmp;
}

cell* ofApp::top(stnode *stack){
    return stack->cell ;
}

/* queue operations */
void ofApp::enque(stnode **queue, stnode **queue_end, cell *item){
    stnode *ptr = (stnode *)malloc(sizeof(stnode *));
    ptr->cell = item;
    ptr->link = NULL;
    
    if((*queue) == NULL) (*queue)=ptr;
    else (*queue_end)->link=ptr;
    (*queue_end) = ptr;
}

cell* ofApp::deque(stnode **queue, stnode **queue_end){
    stnode *ptr = (*queue);
    cell *tmp = ptr->cell;
    if((*queue)->link == NULL )
        (*queue) = (*queue_end) = NULL;
    else
        (*queue)=(*queue)->link;
    free(ptr);
    return tmp;
}

/* path operations */
void ofApp::savePath(path **head, path **pre, cell *a, cell *b){
    path *tmp = (path *)malloc(sizeof(path));
    tmp->w1 = a->w; tmp->h1 = a->h;
    tmp->w2 = b->w; tmp->h2 = b->h;
    tmp->link = NULL;
    if((*head)==NULL)
        (*head) = tmp;
    else
        (*pre)->link = tmp;
    (*pre) = tmp;
}

void ofApp::deletePath(){
    path *ptr=answer_path, *tmp;
    while(ptr!=NULL){
        tmp = ptr;
        ptr = ptr->link;
        free(tmp);
    }
    answer_path = NULL;
    for(int j=0; j<height; j++)
        for(int i=0; i<width; i++)
            cell_ptr[j][i]->visited = false;
    cout<<"path deleted!"<<endl;
}
