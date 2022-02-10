#include "mbed.h"
#include "C12832.h"
#include <stdio.h>
#include <stdlib.h>

/* Inputs */
InterruptIn U(p15);
InterruptIn D(p12);
InterruptIn L(p13);
InterruptIn R(p16);
DigitalIn X(p14);

PwmOut speaker(p26);

int HAUTEUR = 16*2+1;
int LARGEUR = 64*2+1;

float duration = 0.3;
float acceleration = 0.003;
float borneMax = 0.15;

int fruit[2] = {-2,-2};
bool isFruit = false;

int obstacle[2] = {0,0};
int oldObstacle[2] = {0,0};

bool alive = true;
bool menu = true;
bool flip = true; ;

unsigned int score = 1;
int direction = 4;

C12832 LCD(p5, p7, p6, p8, p11);

float coeffH = (LCD.height()/HAUTEUR);
float coeffL = (LCD.width()/LARGEUR);

const int MAX_SNAKE_SIZE = 20;

int snake [MAX_SNAKE_SIZE][2] = { // crée le serpent initial
                    {3,0}, // (x1,y1)
                    {0,0},
                    {0,0},  
                    {0,0},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1},
                    {-1,-1}
                   };
// le serpent est un tableau structuré comme ceci :
// morceau 1 => snake[0], son premier élément est x, son deuxième est y
// x du n morceau est snake[n][0]
// y du n morceau est snake[n][1]
int snakeSize = 3; //taille du serpent, les -1 ne sont pas comptés


    
void afficheSnake(){ //affiche le serpent stocké dans le tableau
    for(int i = 0; i< snakeSize;i++){
     LCD.fillcircle(snake[i][0], snake[i][1], 1, 1);
    }
    LCD.circle(fruit[0], fruit[1], 1, 1);
    LCD.rect(obstacle[0], obstacle[1], obstacle[0]+1, obstacle[1]+1, 1);
    LCD.pixel(oldObstacle[0], oldObstacle[1],0);
    LCD.pixel(oldObstacle[0], oldObstacle[1]+1,0);
    LCD.pixel(oldObstacle[0]+1, oldObstacle[1],0);
    LCD.pixel(oldObstacle[0]+1, oldObstacle[1]+1,0);
}


void grandit(){ // quand on mange un fruit 
if(snakeSize<MAX_SNAKE_SIZE-1) {
        snakeSize++; //on l'agrandit de 1;
        snake[snakeSize-1][0] = snake[snakeSize-2][0];
        snake[snakeSize-1][1] = snake[snakeSize-2][1];
    }
    //on vient mettre les mêmes coordonnées que la queue, pour éviter les problèmes
    //on testera plus tard le déplacement de deux morceaux identiques
    
    score = score*2;//on augemente le score du joueur
    
    isFruit = false;
    fruit[0] = -1;
    fruit[1] = -1;
        
}

void dirUp(){
    if(menu){
        flip = !flip;
    }else{   
        direction = 1;
    }
}
void dirDown(){
    if(menu){
        flip = !flip;
    }else{
        direction = 2;
    }
}
void dirLeft(){
    if(menu){
        if(!flip and borneMax>0.001)borneMax += -0.001;
        else if(acceleration>0.001) acceleration += -0.001;
    }else{
        direction = 3;
    }
}
void dirRight(){
    if(menu){
        if(!flip)borneMax += 0.001;
        else acceleration += 0.001;
    }else{
        direction = 4;
    }
}

void affichePoint(bool up){
    if(up){
        LCD.line(110,5,120,5,1);
        LCD.fillcircle(110,5,1,1);
    }
    else{
        LCD.line(110,13,120,13,1);
        LCD.fillcircle(110,13,1,1);
    }
}


int ajust_3(int x){
    if(x%3==0)  return x;
    if(x%3 == 1) return x+2;
    return x+1;
}

void moveSnake(int direction){ //Up,Down,Left,Right
    
    int oldPos[2] = {snake[0][0],snake[0][1]};
    int actPos[2];
        if(direction==1){
            snake[0][0] = snake[0][0];
            snake[0][1] = (snake[0][1]- 3)%HAUTEUR;
            if(snake[0][1] <0){ // si le joueur sort par la haut
             snake[0][1] += HAUTEUR;   
            }
        }else if(direction == 2){
            snake[0][0] = snake[0][0];
            snake[0][1] = (snake[0][1]+ 3)%HAUTEUR;
        }else if(direction == 3){
            snake[0][0] = (snake[0][0] - 3)%LARGEUR;
            snake[0][1] = snake[0][1];
            if(snake[0][0] <0){ // si le joueur sort par la gauche
             snake[0][0] += LARGEUR;   
            }
        }else{
            snake[0][0] = (snake[0][0]+3)%LARGEUR;
            snake[0][1] = snake[0][1];
        }
        
        for(int i = 1; i<snakeSize;i++){
         actPos[0]= snake[i][0];
         actPos[1]= snake[i][1];
         if(actPos[0] != oldPos[0] or actPos[1] != oldPos[1]){//penser au doublon de corps
             snake[i][0] = oldPos[0];
             snake[i][1] = oldPos[1];
             oldPos[0] = actPos[0];
             oldPos[1] = actPos[1];
         }
         if(i == snakeSize-1){
             //Désaffiche oldPos    
         LCD.pixel(oldPos[0]+1, oldPos[1],0);
         LCD.pixel(oldPos[0]-1, oldPos[1],0);
         LCD.pixel(oldPos[0], oldPos[1]+1,0);
         LCD.pixel(oldPos[0], oldPos[1]-1,0);
        }
        }
    
    //penser au bord de l'écran
}

void isAlive(){
    for(int i = 1; i<snakeSize; i++){
        if(snake[0][0] == snake[i][0] and snake[0][1] == snake[i][1]) alive = false;
        if(snake[0][0] == obstacle[0] and snake[0][1] == obstacle[1]) alive= false;
        if(snake[0][0] == fruit[0] and snake[0][1] == fruit[1]) grandit();
    }
}

bool isEmpty(int x, int y, int type){//sert à savoir si une case contient le snake, l'obstacle ou un fruit
    for(int i =0; i<snakeSize;i++){
        if(x == snake[i][0] and y == snake[i][1]){
            return false;
        }
    }
    if(x == fruit[0] and y == fruit[1] and type != 1 or x == obstacle[0] and y == obstacle[1] and type !=0){
        return false;
    }
    return true;
}

void spawnObstacle(){
    do
    {
        oldObstacle[0] = obstacle[0];
        oldObstacle[1] = obstacle[1];
        
    obstacle[0] = rand()%LARGEUR;   
    obstacle[1] = rand()%HAUTEUR;
    obstacle[0] = ajust_3(obstacle[0]);
    obstacle[1] = ajust_3(obstacle[1]);
    } while(!isEmpty(obstacle[0], obstacle[1],0));// on regarde si la case est bien vide
       
}


void spawnFruit(){
    do
    {
        fruit[0] = rand()%LARGEUR;
        fruit[1] = rand()%HAUTEUR;
        fruit[0] = ajust_3(fruit[0]);
        fruit[1] = ajust_3(fruit[1]);
    } while (!isEmpty(fruit[0],fruit[1],1)); // on regarde si la case est bien vide
    
    isFruit = true;
    spawnObstacle();
       
}




void reset(){
    direction = 4;
    duration = 0.3;
    snakeSize = 3;  
    snake[0][0] = 3;
    snake[0][1] = 0;
    snake[1][0] = 0;
    snake[1][1] = 0;
    snake[2][0] = 0;
    snake[2][1] = 0;

    isFruit = false;
    fruit[0] = -1;
    fruit[1] = -1;
    obstacle[0] = 0;
    obstacle[1] = 0;
    oldObstacle[0] = 0;
    oldObstacle[1] = 0;
    score = 0;
}

int main() {
    //Interupt
    U.rise(&dirUp);
    D.rise(&dirDown);
    L.rise(&dirLeft);
    R.rise(&dirRight);
    while(!X){
        LCD.cls();
        LCD.locate(0,0);
        LCD.printf("Acceleration : %f", acceleration);//affichage
        
        LCD.locate(0,10);
        LCD.printf("Vitesse min : %f", borneMax);//affichage
        
        affichePoint(flip);
        wait(0.2);
    }
    menu = !menu;
    
    
    
    
    
    LCD.cls();
    while(true){
        afficheSnake();
        while(alive){
         
         //add bonus here
            wait(duration);
         //move
            moveSnake(direction);
            
         //Spawn fruit s'il y en a pas, ainsi qu'un obstacle
            if(!isFruit)spawnFruit();
         //display
            afficheSnake();
         //check alive 
            isAlive();
         //reduce duration
            if(duration>borneMax)duration+= -acceleration;
            
        }
        //Fin de partie
            LCD.cls();
            LCD.locate(0,0);
            LCD.printf("Score : %d", score);
            LCD.locate(0,15);
            LCD.printf("Appuyez pour recommencer");
            while(!X){
                wait(0.1);
            }
            reset();
        wait(0.1);
        }
    
}