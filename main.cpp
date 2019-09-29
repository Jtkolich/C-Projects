
/*
Things i did for extra credit: I made cool sounds for bullets and explosions
 I added a score menu and i did my best to clean up the code and add curly braces everywhere
*/
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <list>
#include <cmath>

using namespace sf;
// initialize the width and height of the window
const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;
// create a new animation class
class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	// create a vector called frames, type IntRect
    std::vector<IntRect> frames;
    // animation constructor with no input
    Animation() {}
    // animation constructor with input
    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
	    // frame must be 0 in order for you to see the ship
	    Frame = 0; // start on the zeroth frame of the animation
        speed = Speed; // how long the animation lasts for

		for (int i=0;i<count;i++) {
            // make a new frames object at the back of the vector. intrect creates a rectangle centered at the first two
            // points, with a length and width of the other two points.
            frames.emplace_back(IntRect(x + i * w, y, w, h));
        }

		sprite.setTexture(t);
		sprite.setOrigin(w/2,h/2); // start in the center of the object
        sprite.setTextureRect(frames[0]);
	}


	void update()
	{
    	Frame += speed;
		long long int n = frames.size();
		if (Frame >= n){ // if frame is outside the frames size, decrement the frame
		    Frame -= n;
		}
		if (n>0){
		    sprite.setTextureRect( frames[int(Frame)] ); // if the animation started, play that part of the animation
		}
	}

	bool isEnd()
	{
	  return Frame+speed>=frames.size(); // ensures that the animation is completed
	}

};

// This is the base class
class Entity
{
public:
float x,y,dx,dy,R,angle;
bool life; // life is a true false variable
std::string name; // string called name
Animation anim; // create an animation called anim

Entity() // entity constructor, sets life variable to 1
{
  life=true;
}
// settings dictates the position of the object, its size, and the angle at which it moves
void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
{
  anim = a;
  x=X; y=Y;
  angle = Angle;
  R = radius;
}

virtual void update(){};
// draw function draws the object where it is supposed to be
void draw(RenderWindow &app)
{
  anim.sprite.setPosition(x,y);
  anim.sprite.setRotation(angle+90);
  app.draw(anim.sprite); // draws the sprite on the screen

  CircleShape circle(R); // makes an invisible circle around the object
  circle.setFillColor(Color(255,0,0,170));
  circle.setPosition(x,y);
  circle.setOrigin(R,R);
}
// entity destructor
virtual ~Entity(){};
};

// derived class from Entity
class asteroid: public Entity
{
public:
    static int m_count; // initialize a static variable tht counts the number of asteroids
  asteroid() // constructor sets the asteroid x and y direction to be random
  {
    dx=rand()%8-4;
    dy=rand()%8-4;
    name="asteroid";
    m_count ++; // add 1 asteroid to the count
  }
static int getCount(){
      return(m_count);
  }

void  update() // update the position of the asteroid
  {
   x+=dx;
   y+=dy;

   if (x>W) {
       x=0;
   }
   if (x<0) {
       x=W; // make sure the asteroid doesnt go off of the screen
   }
   if (y>H) {
       y=0;
   }
   if (y<0){
       y=H;
   }
  }
~asteroid(){
     m_count --; // take one asteroid away from the count
  }
};

class ufo: public Entity{
public:
    static int m_count; // count the total number of ufos on the screen
    ufo()
    {
        dx=rand()%5+3; // this is the speed i chose to make sure the spaceship moves decently quick and is hard to hit
        dy=0;
        name = "ufo";
        m_count ++; // add 1 to ufo count

    }
    void  update() // update the position of the asteroid
    {
        x+=dx;
        y+=dy;

        if (x>W) {
            life = false;
        }
        if (x<0) {
            life = false; // make sure the ufo doesnt go off of the screen
        }
        if (y>H) {
            life = false;
        }
        if (y<0) {
            life = false; // if the ufo goes off the screen then it disappears
        }
    }
    static int getCount(){
        return m_count;
    }
    ~ufo(){
        m_count--; // take one away from ufo count
    }
};


// derived class from Entity
class bullet: public Entity
{
public:
  bullet() // bullet constructor makes the name of the entity "bullet'
  {
    name="bullet";
  }

void  update()
  {
   dx=cos(angle*DEGTORAD)*6;
   dy=sin(angle*DEGTORAD)*6;
  // angle+=rand()%6-3;
   x+=dx; // update the position of the bullet
   y+=dy;

   if (x>W || x<0 || y>H || y<0) {
       life=false; // if the bullet goes off of the screen make it not exist
   }
  }

};

// derived class from Entity
class player: public Entity
{
public:
   bool thrust;
    // player constructor initializes name to "player"
   player()
   {
     name="player";
   }
    // update function
   void update()
   {
     if (thrust) // if the player is thrusting the change in x and y direction is designated by this
      { dx+=cos(angle*DEGTORAD)*0.2; // x and y acceleration
        dy+=sin(angle*DEGTORAD)*0.2; }
     else // slows down the player if not thrusting
      { dx*=0.95;
        dy*=0.95; }

    int maxSpeed=15; // sets the max speed
    double speed = sqrt(dx*dx+dy*dy); // simple speed function
    if (speed>maxSpeed) // make sure the speed cannot go above the max speed
     { dx *= maxSpeed/speed; // this is essentially 1
       dy *= maxSpeed/speed; }

    x+=dx; // change in x of player
    y+=dy; // change in y of player

    if (x>W) {
        x=0;
    }
    if (x<0) {
        x=W; // make sure the player doesnt go off of the screen
    }
    if (y>H){
        y=0;
    }
    if (y<0) {
        y=H;
    }
   }

};


bool isCollide(Entity *a,Entity *b)
{ // if the distance between the center two objects is less than the radius of the two combined objects, there is a
    // collision that has happened
  return (b->x - a->x)*(b->x - a->x)+ (b->y - a->y)*(b->y - a->y)< (a->R + b->R)*(a->R + b->R);
}

int asteroid::m_count =0;
int ufo::m_count = 0;
int main()
{
    int score = 0;
    int lives = 1;
    srand(time(0));
    RenderWindow app(VideoMode(W, H), "Asteroids!"); // name of the application
    app.setFramerateLimit(60); // framerate of the application capped at 60

    SoundBuffer buffer, buffer2, buffer3; // make sound objects that will be played at different times
    buffer.loadFromFile("sounds/Flashbang.ogg");
    buffer2.loadFromFile("sounds/ufo.ogg");
    buffer3.loadFromFile("sounds/pew.ogg");
    Sound collision;
    Sound ufoSound;
    Sound bulletSound;
    collision.setBuffer(buffer); // Flashbang by Kibblesbob from http://soundbible.com/1793-Flashbang.html Public Domain
    ufoSound.setBuffer(buffer2); // UFO Landing by Sonidor from http://soundbible.com/2063-UFO-Landing.html Attribution 3.0
    bulletSound.setBuffer(buffer3); // Bullet sound by Mike Koenig from http://soundbible.com/1771-Laser-Cannon.html Attribution 3.0

    Font font;
    font.loadFromFile("fonts/data-latin.ttf");
    Text text, text2; // text 1 for score, text 2 for lives
    text.setFont(font);
    text2.setFont(font);

    Texture t1,t2,t3,t4,t5,t6,t7,t8,t9; // creates all of the pictures you see on the screen
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    // <div>Icons made by <a href="https://www.freepik.com/"
    // title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/"
    // title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/"
    //    title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
    t8.loadFromFile("images/ufo.png");
    t9.loadFromFile("images/gameover.png");

    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite background(t2),sGameOver(t9);
    // creates all of the animatons
    Animation sExplosion(t3, 0,0,256,256, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);
    Animation sUFO(t8,0,0,64,64,1,0.0);
    sGameOver.setPosition(350,300);


    std::list<Entity*> entities;

    for(int i=0;i<15;i++)
    {
      // create a memory location for a new asteroid
      auto *a = new asteroid();
      a->settings(sRock, rand()%W, rand()%H, rand()%360, 25); // make the new asteroid have these movement traits
      entities.push_back(a); // make the asteroid at the end of the entities vector

    }

    auto *p = new player(); // allocate memory for a new player
    p->settings(sPlayer,W/2,H/2,0,20);// I changed this to make the player start in the middle
    entities.push_back(p);// add p to the end of the entities vector

    /////main loop/////
    while (app.isOpen()) // while the
    {
        Event event; // make a new event called event.
        while (app.pollEvent(event))  //returns true if an event is queued (it always will be)
        {
            if (event.type == Event::Closed) {// close the window if the x is pressed
                app.close();
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Space) // if a the space bar is pressed, shoot a bullet
                {
                    auto *b = new bullet(); // allocate space for a new bullet object
                    b->settings(sBullet, p->x, p->y, p->angle,
                                10); // create a bullet from where the player is currently
                    entities.push_back(b); // add a bullet to the end of the entities list
                    bulletSound.play();
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            p->angle += 3; // change the angle of the player if the right or
        }
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            p->angle -= 3; // left keys are pressed
        }
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            p->thrust = true; // thrust if the up key is pressed
        } else {
            p->thrust = false; // if none of these are true then dont thrust
        }


        for (auto a:entities) { // cycle through the list of entities
            for (auto b:entities) { // cycle through the list of entities
                if (a->name == "asteroid" && b->name == "bullet") { // if a bullet collides with a asteroid
                    if (isCollide(a, b)) {
                        collision.play();// play the collision sound
                        score += 10; // add 10 to the score
                        a->life = false; // destroy the asteroid
                        b->life = false; // destroy the bullet

                        auto *e = new Entity(); // allocate memory for a new entity, "e"
                        e->settings(sExplosion, a->x, a->y); // create a location at the spot where the objects collided
                        e->name = "explosion";
                        entities.push_back(e); // add an explosion the the entities list


                        for (int i = 0; i < 2; i++) {
                            if (a->R == 15) continue; // if the radius of the asteroid is 15, leave the for loop
                            Entity *e = new asteroid(); // allocate memory for a new asteroid object
                            e->settings(sRock_small, a->x, a->y, rand() % 360,
                                        15); // make two small rocks where the asteroid was
                            entities.push_back(e); // add the two asteroids the the entities vector
                        }

                    }
                }
                if (a->name == "ufo" && b->name == "bullet") { // if a bullet collides with a ufo
                    if (isCollide(a, b)) {
                        collision.play();// play collision sound
                        score += 200; // ufos are worth more points because they're rare
                        a->life = false; // destroy the ufo
                        b->life = false; // destroy the bullet

                        auto *e = new Entity(); // allocate memory for a new entity, "e"
                        e->settings(sExplosion, a->x, a->y); // create a location at the spot where the objects collided
                        e->name = "explosion";
                        entities.push_back(e); // add an explosion the the entities list
                    }
                }

                if (a->name == "player" && b->name == "asteroid") { // if a player collides with an asteroid
                    if (isCollide(a, b)) {
                        collision.play();
                        lives--;
                        b->life = false; // destroy the asteroid

                        auto *e = new Entity(); // allocate memory for a new entity
                        e->settings(sExplosion_ship, a->x,a->y); // set a ship explosion where the ship collided with the asteroid
                        e->name = "explosion";
                        entities.push_back(e); // put the e entity at the end of the entities vector

                        p->settings(sPlayer, W / 2, H / 2, 0, 20); // reset the player
                        p->dx = 0;
                        p->dy = 0; // make the players movement vectors 0
                    }
                }
                if (a->name == "player" && b->name == "ufo") { // if a player collides with an ufo
                    if (isCollide(a, b)) {
                        collision.play();
                        lives--;
                        b->life = false; // destroy the ufo

                        auto *e = new Entity(); // allocate memory for a new entity
                        e->settings(sExplosion_ship, a->x,
                                    a->y); // set a ship explosion where the ship collided with the asteroid
                        e->name = "explosion";
                        entities.push_back(e); // put the e entity at the end of the entities vector

                        p->settings(sPlayer, W / 2, H / 2, 0, 20); // reset the player
                        p->dx = 0;
                        p->dy = 0; // make the players movement vectors 0
                    }
                }

            }
        }


        if (p->thrust) {
            p->anim = sPlayer_go; // if thrusting, then initiate the thrusting animation
        } else {
            p->anim = sPlayer; // otherwise just do the normal player animation
        }


        for (auto e:entities) { // loop through the list of entities
            if (e->name == "explosion") { // if the entity name is explosion
                if (e->anim.isEnd()) { e->life = false; } // if the animation is over, then life is false
            }
        }

        /*if (rand()%150==0) //if a random generated number is a multiple of 150 then create an asteroid
         {
           auto *a = new asteroid(); // allocate memory for a new asteroid
           a->settings(sRock, 0,rand()%H, rand()%360, 25); // make the new asteroid a big asteroid
           entities.push_back(a); // add the new asteroid to the entities vector
         }*/
        if (asteroid::getCount() == 0) {
            for (int i = 0; i < 15; i++) {
                // create a memory location for a new asteroid
                auto *a = new asteroid();
                a->settings(sRock, rand() % W, rand() % H, rand() % 360,
                            25); // make the new asteroid have these movement traits
                entities.push_back(a); // make the asteroid at the end of the entities vector

            }
        }
        if (rand() % 5000 == 0 and
            (ufo::getCount() == 0)) //if a random generated number is a multiple of 5000, can only be 1 ufo
        {
            ufo *a = new ufo(); // allocate memory for a new ufo
            a->settings(sUFO, 0, rand() % H, 0, 15);
            entities.push_back(a); // add the new asteroid to the entities vector
            ufoSound.play(); // play the ufo sound
        }

        for (auto i = entities.begin(); i != entities.end();) // loop through all of the entities in the vector
        {
            Entity *e = *i; // cycles through each entity

            e->update(); // update the position and speed of the entity
            e->anim.update(); // update the animation

            if (!e->life) {
                i = entities.erase(i);
                delete e;
            } // if the life variable is false then delete the entity, otherwise
            else i++; // move on the the next entity
        }
        if (lives < 10) {
            score = 0; // prevents you from scoring if you lose too many lives, i cant figure out how to get the program
            // to completely pause like it did in hw5 though.
        }




        //////draw//////
        app.draw(background); // draw the background
        text.setString("Score: " + std::to_string(score));
        text.setCharacterSize(30);
        text.setFillColor(Color::White);
        text.setOutlineColor(Color::Black);
        text.setOutlineThickness(1);
        app.draw(text);
        text2.setString("Lives: " + std::to_string(lives));
        text2.setCharacterSize(30);
        text2.setPosition(500, 0);
        text2.setFillColor(Color::White);
        text2.setOutlineColor(Color::Black);
        text2.setOutlineThickness(1);
        app.draw(text2);

        for (auto i:entities) { // for each entity draw and display it in the application
            i->draw(app);
        }
        if (lives < 1) {
            app.draw(sGameOver);
            score = 0; // if you lose all your lives then you cant score and nothing really counts for anything,
            lives = 0;// just restart the game
        }
        app.display();
    }





    return 0;
}
