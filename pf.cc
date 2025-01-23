#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <vector>


SDL_Window* window = nullptr;
SDL_Renderer * renderer = nullptr;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PATH_LENGTH = 30;

bool isDragging = false;
const double GRAVITY = 0.5;
const double DAMPING = 0.996;

//Graph constants
const int GRAPH_HEIGHT = 200;
const int GRAPH_WIDTH = 400;
std::vector<double> angleHistory;
std::vector<double> kinetic;
std::vector<double> potential;

//pendulum strct 
struct Pendulum{
  double originX;
  double originY;
  double string_length;
  double displaced_angle;
  double ang_velocity;
  double ang_acceleration;

  double getBobX(){
    return (originX + string_length*sin(displaced_angle));
  }
  double getBobY(){
    return (originY + string_length*cos(displaced_angle));
  }
};
struct PathPoint {
    int x, y;
    short red, green, blue;
    
    PathPoint(int a, int b,int r,int g,int bl) : x(a), y(b), red(r), green(g), blue(bl) {}
    
};
std::vector<PathPoint*> trace_path_container;
//pendulum
Pendulum pendulum = {400,100,180,M_PI/4,0,0};

bool SDLINIT(){
  if(SDL_Init(SDL_INIT_VIDEO)<0){
    std::cerr<<"Error!"<<SDL_GetError()<<"\n";
    return false;
  }
  window = SDL_CreateWindow("Pendulum",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if(window == nullptr){
    std::cerr<<"Error!"<<SDL_GetError()<<"\n";
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_ACCELERATED);
  if(renderer == nullptr){
    std::cerr<<"Error!"<<SDL_GetError()<<"\n";
    return false;
  }
  return true;
}

void manageTracePath() {
    // Remove oldest point if the path is full or acceleration is zero
    if (trace_path_container.size() >= PATH_LENGTH || 
        pendulum.ang_velocity == 0) {
        delete trace_path_container.front();
        trace_path_container.erase(trace_path_container.begin());
    }

    // Add new point if path is not full
    if (trace_path_container.size() < PATH_LENGTH) {
        // Get bob coordinates
        int x = pendulum.getBobX();
        int y = pendulum.getBobY();

        // Generate colour based on coordinates
        short red = static_cast<short>((x*10 % 255 + y % 255));
        short green = static_cast<short>((x * y) % 255);
        short blue = static_cast<short>((x + y) % 255);

        // Create and add a new path point
        PathPoint* newPoint = new PathPoint(x, y, red, green, blue);
        trace_path_container.push_back(newPoint);
    }
}

void graphUpdate(){
  angleHistory.push_back(pendulum.displaced_angle);
  double k2 = 0.5*(pendulum.ang_velocity * pendulum.ang_velocity)*(pendulum.string_length*pendulum.string_length);
  double p1 = 9.8*pendulum.string_length*(1 - cos(pendulum.displaced_angle));
  kinetic.push_back(k2);
  potential.push_back(p1);
  if(angleHistory.size() > GRAPH_WIDTH){
    angleHistory.erase(angleHistory.begin());
  }
  if(kinetic.size() > GRAPH_WIDTH - 60){
    kinetic.erase(kinetic.begin());
  }
  if(potential.size() > GRAPH_WIDTH - 60){
    potential.erase(potential.begin());
  }
}

void updatePendulum() {
  if(!isDragging){
        // Calculate angular acceleration using pendulum equation
    double ang_acc = -(GRAVITY / pendulum.string_length) * sin(pendulum.displaced_angle);
    // Update angular velocity and apply damping
    pendulum.ang_acceleration = ang_acc;
    pendulum.ang_velocity += ang_acc;
    pendulum.ang_velocity *= DAMPING;
    // Update displaced angle
    pendulum.displaced_angle += pendulum.ang_velocity;
    // Trace path management
    manageTracePath();
    graphUpdate();
  }else{
    int mx, my;
    SDL_GetMouseState(&mx,&my);
    double dx = mx - pendulum.originX;
    double dy = my - pendulum.originY;
    pendulum.displaced_angle = atan2(dx,dy);
    angleHistory.erase(angleHistory.begin());
    kinetic.erase(kinetic.begin());
    potential.erase(potential.begin());
  }
}

void renderLabel(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color textColor, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (!surface) {
        std::cerr << "Failed to create text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create text texture! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    // Cleanup
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void energyGraph(){

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " 
                  << TTF_GetError() << std::endl;
        return;
    }

    // Calculate max values for dynamic scaling
    double maxKinetic = 0;
    double maxPotential = *std::max_element(potential.begin(), potential.end());
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_Rect graphRect = {SCREEN_WIDTH - GRAPH_WIDTH +40, SCREEN_HEIGHT - GRAPH_HEIGHT - 25, GRAPH_WIDTH - 60, GRAPH_HEIGHT};
    SDL_RenderDrawRect(renderer,&graphRect);

  SDL_SetRenderDrawColor(renderer, 101,101, 70, 255);
  for (size_t i = 1; i < kinetic.size(); ++i) {

      int x1 = SCREEN_WIDTH - GRAPH_WIDTH + 40 + i - 1;
      int y1 = (SCREEN_HEIGHT - GRAPH_HEIGHT - 35) + GRAPH_HEIGHT - (kinetic[i - 1])*1.5; // Scale angle to fit graph
      int x2 = SCREEN_WIDTH - GRAPH_WIDTH + 40 + i;
      int y2 = (SCREEN_HEIGHT - GRAPH_HEIGHT - 35) + GRAPH_HEIGHT - (kinetic[i])*1.5;
      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
      if(i% 2 == 0){
        maxKinetic = kinetic[i];
      }
  }
  //POTENTIAL ENERGY
  SDL_SetRenderDrawColor(renderer, 50,255, 50, 255);
  for (size_t i = 1; i < potential.size(); ++i) {
      int x1 = SCREEN_WIDTH - GRAPH_WIDTH + 40 + i - 1;
      int y1 = (SCREEN_HEIGHT - GRAPH_HEIGHT - 35) + GRAPH_HEIGHT - (potential[i - 1]/6); // Scale angle to fit graph
      int x2 = SCREEN_WIDTH - GRAPH_WIDTH + 40 + i;
      int y2 = (SCREEN_HEIGHT - GRAPH_HEIGHT - 35) + GRAPH_HEIGHT - (potential[i]/6);
      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
      if(i% 2 == 0){
        maxPotential = potential[i];
      }
  }
  //total energy
  SDL_SetRenderDrawColor(renderer, 255,101, 50, 255);
  for (size_t i = 1; i < potential.size() && i < kinetic.size(); ++i) {
    double t1 = kinetic[i -1] * 1.5 + potential[i -1]/6;
    double t2 = kinetic[i] *1.5+ potential[i]/6;
    int x1 = SCREEN_WIDTH - GRAPH_WIDTH + 40 + i - 1;
    int y1 = (SCREEN_HEIGHT - GRAPH_HEIGHT - 35) + GRAPH_HEIGHT - (int)(t1); // Scale angle to fit graph
    int x2 = SCREEN_WIDTH - GRAPH_WIDTH + 40 + i;
    int y2 = (SCREEN_HEIGHT - GRAPH_HEIGHT - 35) + GRAPH_HEIGHT - (int)(t2);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
  }

  if (TTF_WasInit() || TTF_Init() != -1) {
      TTF_Font* font = TTF_OpenFont("arial.ttf", 12);
      if (font) {
          SDL_Color textColor = {255, 255, 255, 255};

          // Render labels
          renderLabel(renderer, font, " Kinetic: " + std::to_string(static_cast<int>(maxKinetic)), 
                      textColor, SCREEN_WIDTH - GRAPH_WIDTH + 50, SCREEN_HEIGHT - GRAPH_HEIGHT - 10);

          renderLabel(renderer, font, " Potential: " + std::to_string(static_cast<int>(maxPotential)), 
                      textColor, SCREEN_WIDTH - GRAPH_WIDTH + 50, SCREEN_HEIGHT - GRAPH_HEIGHT + 10);

          renderLabel(renderer, font, " Total Energy: " + std::to_string(static_cast<int>(maxPotential + maxKinetic)), 
                      textColor, SCREEN_WIDTH - GRAPH_WIDTH + 50, SCREEN_HEIGHT - GRAPH_HEIGHT + 30);

          // Cleanup
          TTF_CloseFont(font);
      } else {
          std::cerr << "Font could not be loaded! SDL_ttf Error: " << TTF_GetError() << std::endl;
      }
  }

}


void renderCircle(int centerX, int centerY, int radius){
  int dx,  dy;
  for(int i = 0; i < radius*2; i++){
    for(int j = 0; j < radius*2; j++){
      dx = radius - i;
      dy = radius - j;
      if((dx*dx + dy*dy)<=radius*radius){
        SDL_RenderDrawPoint(renderer,centerX+dx,centerY+dy);
      }
    }
  }

}
void renderFilledCircle( int centerX, int centerY, int radius) {
    const int diameter = (radius * 2);

    int x = radius - 1;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int error = (tx - diameter);

    while (x >= y) {
        // Draw horizontal lines
        for (int i = centerX - x; i <= centerX + x; i++) {
            SDL_RenderDrawPoint(renderer, i, centerY - y);
            SDL_RenderDrawPoint(renderer, i, centerY + y);
        }

        for (int i = centerX - y; i <= centerX + y; i++) {
            SDL_RenderDrawPoint(renderer, i, centerY - x);
            SDL_RenderDrawPoint(renderer, i, centerY + x);
        }

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

void renderGraph(){
  SDL_SetRenderDrawColor(renderer, 255,255,255,255);
  SDL_Rect graphRect = {25, SCREEN_HEIGHT - GRAPH_HEIGHT - 25, GRAPH_WIDTH, GRAPH_HEIGHT};
  SDL_RenderDrawRect(renderer,&graphRect);
  double pos = 0;

  SDL_SetRenderDrawColor(renderer,0,255,255,255);
  for(size_t i =1; i < angleHistory.size(); i++){
    int x1 = 25 + i - 1;
    int y1 = (SCREEN_HEIGHT - GRAPH_HEIGHT/2 - 25)  - (angleHistory[i - 1]*48);
    int x2 = 25 + i;
    int y2 = (SCREEN_HEIGHT - GRAPH_HEIGHT/2 - 25) - (angleHistory[i] * 48);
    SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
    if(i%2){
      pos = angleHistory[i];
    }
  }
  energyGraph();

  if (TTF_WasInit() || TTF_Init() != -1) {
      TTF_Font* font = TTF_OpenFont("arial.ttf", 12);
      if (font) {
          SDL_Color textColor = {255, 255, 255, 255};
          renderLabel(renderer, font, "Position: " + std::to_string(static_cast<int>(57.2958*pos))+" deg", 
                      textColor, 40, SCREEN_HEIGHT - GRAPH_HEIGHT - 5);
          // Cleanup
          TTF_CloseFont(font);
      } else {
          std::cerr << "Font could not be loaded! SDL_ttf Error: " << TTF_GetError() << std::endl;
      }
  }

}

//render functions
void RenderScreen(){
  //clear the screen
  SDL_SetRenderDrawColor(renderer,0,0,0,255);
  SDL_RenderClear(renderer);
  //rendering the components
  //render string
  SDL_SetRenderDrawColor(renderer,255,255,255,255);
  SDL_RenderDrawLine(renderer,pendulum.originX,pendulum.originY, pendulum.getBobX(),pendulum.getBobY());
  //render bob or mass
  SDL_SetRenderDrawColor(renderer,55,200,0,255);
  // SDL_Rect bob = {(int)(pendulum.getBobX()) - 10,(int)(pendulum.getBobY()) - 10, 20,20 };
  // SDL_RenderFillRect(renderer,&bob);
  renderFilledCircle((int)(pendulum.getBobX()),(int)(pendulum.getBobY()),10);
  //render path
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

  for(size_t i = 1; i < trace_path_container.size();i++){
    //std::cout<<trace_path_container[i]->red<<"\n";
    SDL_SetRenderDrawColor(renderer,trace_path_container[i]->red,trace_path_container[i]->green,trace_path_container[i]->blue,255);
    SDL_RenderDrawLine(renderer,trace_path_container[i-1]->x,trace_path_container[i-1]->y,trace_path_container[i]->x, trace_path_container[i]->y);
  }

  //render graph
  renderGraph();
  //calling the render handler
  SDL_RenderPresent(renderer);
}
void cleanupTracePath() {
    for (auto point : trace_path_container) {
        delete point;
    }
    trace_path_container.clear();
}

void DestroyScreen(){
  cleanupTracePath();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();  
  SDL_Quit();
}

void handleMouseEvents(const SDL_Event& e){
  if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT){
    int mx;
    int my;
    SDL_GetMouseState(&mx,&my);
    int bobX = pendulum.getBobX();
    int bobY = pendulum.getBobY();
    double distance = sqrt((mx - bobX)*(mx - bobX)+(my - bobY)*(my - bobY));

    if(distance < 20){
      pendulum.ang_velocity += (mx < bobX)? 0.1 : -0.1;
    }else{
      isDragging = true;
    }
  }else if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT){
    isDragging = false;
  }
}

int main(){

  if(!SDLINIT()){
    return -1;
  }
  //Events
  SDL_Event e;
  bool quit = false;

  Uint32 frameStart,frameEnd;
  while(!quit){
    frameStart = SDL_GetTicks();
    while(SDL_PollEvent(&e) != 0){
      if(e.type == SDL_QUIT){
        quit = true;
      }

      if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_ESCAPE){
          quit = true;
        }
      }
      handleMouseEvents(e);
    }
    updatePendulum();
    RenderScreen();  
    frameEnd = SDL_GetTicks() - frameStart;
    if(frameEnd < 25){
      SDL_Delay(25 - frameEnd);
    }
  }

  DestroyScreen();
  return 0;
}
