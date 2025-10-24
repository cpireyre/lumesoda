#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <ctime>
#include <cstdbool>

# define WINDOW_WIDTH        1280
# define WINDOW_HEIGHT       720
# define PARTICLE_COUNT      50
# define PLAYER_SPEED        450.0f // Pixels per second
# define PLAYER_RADIUS       15
# define PARTICLE_RADIUS     5
# define GRAVITY             100.0f // Pixels per second
# define SAFE_ZONE_WIDTH     100


typedef enum
{
  PLAYING,
  GAME_WON,
  GAME_OVER,
} GameState;

typedef struct
{
  bool show;
  sf::Text *text;
} TextOnTheScreen;

typedef struct
{
  sf::Vector2<float> player;
  sf::CircleShape    playerCircle;
  sf::Vector2<float> particles[PARTICLE_COUNT];
  sf::Vector2<float> particlesVelocity[PARTICLE_COUNT];
  sf::CircleShape    particlesCircle[PARTICLE_COUNT];
  sf::RectangleShape safeZone;
  sf::RectangleShape winZone;
  GameState          state;
  TextOnTheScreen    instructions;
  TextOnTheScreen    gameOver;
  TextOnTheScreen    gameWon;
  TextOnTheScreen    scoreText;
  int                score;
} Game;

void initGame(Game *G)
{
  for (int i = 0; i < PARTICLE_COUNT; ++i)
  {
    G->particles[i].x = SAFE_ZONE_WIDTH + (rand() % (WINDOW_WIDTH - 2 * SAFE_ZONE_WIDTH));
    G->particles[i].y = rand() % WINDOW_HEIGHT;
    G->particlesVelocity[i].y = 1 + (rand() % 2);
    G->particlesCircle[i] = sf::CircleShape(PARTICLE_RADIUS);
    G->particlesCircle[i].setPosition(G->particles[i]);
    G->particlesCircle[i].setOutlineColor(sf::Color::Red);
    G->particlesCircle[i].setOutlineThickness(1.0f);
  }
  G->player = {SAFE_ZONE_WIDTH / 2, WINDOW_HEIGHT / 2};
  G->state = PLAYING;
  G->gameOver.show = false;
  G->gameWon.show = false;
  G->scoreText.show = false;
}
float delta_time_s(void)
{
  static sf::Clock c;
  return (c.restart().asSeconds());
}

sf::Vector2<float> clamp(sf::Vector2<float> p, sf::Vector2<float> bounds)
{
  if (p.x > bounds.x) p.x = bounds.x;
  if (p.x < 0) p.x = 0;
  if (p.y > bounds.y) p.y = bounds.y;
  if (p.y < 0) p.y = 0;
  return (p);
}

sf::Vector2<float> normalize(sf::Vector2<float> u) {
  float norm = sqrt(u.x * u.x + u.y * u.y);
  u.x /= norm; u.y /= norm;
  return (u);
}
void movePlayer(Game *G, float dt_s)
{
  float v_pps = PLAYER_SPEED; // Pixels per second
  sf::Vector2<float> dp = {0, 0};
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
      || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    dp.x++;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
      || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    dp.x--;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
      || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    dp.y--;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
      || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    dp.y++;
  if (dp.x || dp.y) dp = normalize(dp);
  G->player += dp * dt_s * v_pps;
  G->player = clamp(G->player, {WINDOW_WIDTH, WINDOW_HEIGHT});
  G->playerCircle.setPosition(G->player);
}


float square(float x) { return (x * x); }
bool  collide(Game *G)
{
  for (int i = 0; i < PARTICLE_COUNT; i++)
  {
    if (G->playerCircle.getGlobalBounds().contains(G->particles[i]))
      return (true);
  }
  return (false);
}

void  rain(Game *G, float dt_s)
{
  float v_pps = GRAVITY; // Pixels per second
  const float speed = 100.0f;
  const float amplitude = 100.0f;
  sf::Vector2<float> dp = {0, GRAVITY};
  for (int i = 0; i < PARTICLE_COUNT; ++i)
  {
    dp.x = cos(G->particles[i].y / speed) * amplitude;
    G->particles[i] += dp * dt_s * G->particlesVelocity[i].y;
    if (G->state == PLAYING && G->particles[i].y > WINDOW_HEIGHT) G->particles[i].y = 0;
    G->particlesCircle[i].setPosition(G->particles[i]);
  }
}

void  update(Game *G)
{
  float dt_s = delta_time_s();
  switch (G->state)
  {
    case PLAYING:
      {
        movePlayer(G, dt_s);
        rain(G, dt_s);
        G->playerCircle.setFillColor(sf::Color::White);
        if (G->safeZone.getGlobalBounds().contains(G->player))
          break;
        if (G->winZone.getGlobalBounds().contains(G->player))
        {
          G->state = GAME_WON;
          G->score += 1;
          char buffer[100];
          snprintf(buffer, sizeof(buffer), "Score: %d", G->score);
          G->scoreText.text->setString(buffer);
        }
        else if (collide(G))
        {
          G->playerCircle.setFillColor(sf::Color::Red);
          G->state = GAME_OVER;
          G->score = 0;
        }
        break;
      }
    case GAME_OVER:
      {
        G->instructions.show = false;
        G->gameWon.show = false;
        G->gameOver.show = true;
        //rain(G, dt_s);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
          initGame(G);
        break;
      }
    case GAME_WON:
      {
        rain(G, dt_s);
        G->instructions.show = false;
        G->gameOver.show = false;
        G->gameWon.show = true;
        G->scoreText.show = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
          initGame(G);
        break;
      }
  }
}


int main()
{

sf::Clock          clocc;
sf::Clock          shaderClock;
sf::Font           font; font.loadFromFile("DelaGothicOne-Regular.ttf");
sf::Shader         bgShader;
sf::RectangleShape fullScreenQuad;

  srand(time(NULL));
  Game G;
  fullScreenQuad.setSize({WINDOW_WIDTH, WINDOW_HEIGHT});
  fullScreenQuad.setPosition({0,0});
  if (!bgShader.loadFromFile("bgShader.frag",sf::Shader::Type::Fragment))
    return (1);
  G.score = 0;
  G.instructions.text = new sf::Text("Get to the other side!", font);
  G.instructions.text->setCharacterSize(50);
  G.instructions.text->setFillColor(sf::Color::Red);
  G.instructions.text->setStyle(sf::Text::Bold);
  G.instructions.text->setPosition({WINDOW_WIDTH / 4, 0});
  G.instructions.show = true;
  G.gameWon.text = new sf::Text("We did it!", font);
  G.gameWon.text->setCharacterSize(100);
  G.gameWon.text->setFillColor(sf::Color::Green);
  G.gameWon.text->setStyle(sf::Text::Bold);
  G.gameWon.text->setPosition({WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2});
  G.scoreText.text = new sf::Text("",font);
  G.scoreText.text->setPosition({WINDOW_WIDTH / 3, 500});
  G.instructions.show = true;
  G.gameOver.text = new sf::Text("Bummer!", font);
  G.gameOver.text->setCharacterSize(100);
  G.gameOver.text->setFillColor(sf::Color::White);
  G.gameOver.text->setStyle(sf::Text::Bold);
  G.gameOver.text->setPosition({WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2});
  G.gameOver.show = false;
  G.playerCircle = sf::CircleShape(PLAYER_RADIUS);
  G.playerCircle.setOutlineColor(sf::Color::Blue);
  G.playerCircle.setOutlineThickness(5.0f);
  initGame(&G);
  G.safeZone = sf::RectangleShape({SAFE_ZONE_WIDTH, WINDOW_HEIGHT});
  G.safeZone.setPosition({0,0});
  G.safeZone.setFillColor(sf::Color::Green);
  G.winZone = sf::RectangleShape({SAFE_ZONE_WIDTH, WINDOW_HEIGHT});
  G.winZone.setPosition({WINDOW_WIDTH - SAFE_ZONE_WIDTH, 0});
  G.winZone.setFillColor(sf::Color::Blue);

  sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH,WINDOW_HEIGHT}), "window");
  clocc.restart();
  shaderClock.restart();
  while (window.isOpen())
  {
    update(&G);
    window.clear(sf::Color::Black);
    bgShader.setUniform("time", shaderClock.getElapsedTime().asSeconds());
    bgShader.setUniform("resolution", sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bgShader.setUniform("player", G.player);
    window.draw(fullScreenQuad, &bgShader);
    if (G.instructions.show) window.draw(*G.instructions.text);
    if (G.gameOver.show)     window.draw(*G.gameOver.text);
    if (G.gameWon.show)      window.draw(*G.gameWon.text);
    if (G.scoreText.show)    window.draw(*G.scoreText.text);
    for (int i = 0; i < PARTICLE_COUNT; ++i) window.draw(G.particlesCircle[i]);
    window.draw(G.safeZone);
    window.draw(G.winZone);
    window.draw(G.playerCircle);
    window.display();
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Escape))
        window.close();
    }
  }
  delete G.instructions.text;
  delete G.gameOver.text;
  delete G.gameWon.text;
  delete G.scoreText.text;
}
