//Simple matrix transform

constexpr const float GRID_INVERSE_WIDTH = 1.f / (2.f * GRID_WIDTH);
constexpr const float GRID_INVERSE_HEIGHT = 1.f / (2.f * GRID_HEIGHT);

inline float BasisChange::pixelToGridX(float x, float y)
{
	return GRID_INVERSE_WIDTH * x - GRID_INVERSE_HEIGHT * y;
}

inline float BasisChange::pixelToGridY(float x, float y)
{
	return GRID_INVERSE_WIDTH * x + GRID_INVERSE_HEIGHT * y;
}

inline float BasisChange::gridToPixelX(float x, float y)
{
	return GRID_WIDTH * (x + y);
}

inline float BasisChange::gridToPixelY(float x, float y)
{
	return GRID_HEIGHT * (y - x);
}

inline sf::Vector2f BasisChange::pixelToGrid(float x, float y)
{
	return sf::Vector2f(pixelToGridX(x, y), pixelToGridY(x, y));
}

inline sf::Vector2f BasisChange::gridToPixel(float x, float y)
{
	return sf::Vector2f(gridToPixelX(x, y), gridToPixelY(x, y));
}

inline float BasisChange::pixelToGridX(const sf::Vector2f &vec)
{
	return GRID_INVERSE_WIDTH * vec.x - GRID_INVERSE_HEIGHT * vec.y;
}

inline float BasisChange::pixelToGridY(const sf::Vector2f &vec)
{
	return GRID_INVERSE_WIDTH * vec.x + GRID_INVERSE_HEIGHT * vec.y;
}

inline float BasisChange::gridToPixelX(const sf::Vector2f &vec)
{
	return GRID_WIDTH * (vec.x + vec.y);
}

inline float BasisChange::gridToPixelY(const sf::Vector2f &vec)
{
	return GRID_HEIGHT * (vec.y - vec.x);
}

inline float BasisChange::gridToPixelX(const sf::Vector2u &vec)
{
	return gridToPixelX(vec.x, vec.y);
}

inline float BasisChange::gridToPixelY(const sf::Vector2u &vec)
{
	return gridToPixelY(vec.x, vec.y);
}

inline sf::Vector2f BasisChange::pixelToGrid(const sf::Vector2f &vec)
{
	return sf::Vector2f(pixelToGridX(vec), pixelToGridY(vec));
}

inline sf::Vector2f BasisChange::gridToPixel(const sf::Vector2f &vec)
{
	return sf::Vector2f(gridToPixelX(vec), gridToPixelY(vec));
}

inline sf::Vector2f BasisChange::gridToPixel(const sf::Vector2u &vec)
{
	return sf::Vector2f(gridToPixelX(vec), gridToPixelY(vec));
}
