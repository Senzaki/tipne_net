inline bool DrawableEntity::isDepthLower(const DrawableEntity *a, const DrawableEntity *b)
{
	return a->getDepth() < b->getDepth();
}
