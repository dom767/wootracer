#pragma once

#include <set>
#include <vector>
#include <map>
#include "Vector3.h"
#include "AABoundingBox.h"
#include "Ray.h"

class DRenderObject;
class DKDTreeNode;
struct DCollisionResponse;
class DRenderCache;
struct DRayContext;
class DScene;

class DKDTreeNode
{
public:
	DKDTreeNode() : mLeft(NULL), mRight(NULL) {}
	~DKDTreeNode() {if (mLeft) delete mLeft; if (mRight) delete mRight;}
	std::vector<DRenderObject*> mObjects;
	DKDTreeNode *mLeft, *mRight;
	DVector3 mPoint;
};

class DKDTree
{
public:
	DKDTree() : mRootNode(0), mRecursions(0), mIntersections(0) {}
	~DKDTree() {if (mRootNode) delete mRootNode;}

	void MapObjects( std::map<DRenderObject*, DRenderObject*>& ObjectMap );

	void Initialise(const std::vector<DRenderObject*>& objects);
	void InitialiseSAH(const std::vector<DRenderObject*>& objects);
	void InitialiseSimple(const std::vector<DRenderObject*>& objects);
	int Intersect(const DRayContext& rRayContext, DCollisionResponse& out_Response, bool debuginfo) const;
	void SetWithin(DRayContext& rRayContext) const;
	mutable __int64 mRecursions;
	mutable __int64 mIntersections;

private:
	float	 EvaluateCost(const float splitPosition,
		const std::vector<DRenderObject*>& objects,
		const DAABoundingBox& boundingBox,
		EAxis axis);
	void GetAllSplitPositions(const std::vector<DRenderObject*>& objects,
		std::set<float> &splitPositions,
		EAxis axis);
	void CreateNodeSAH(DKDTreeNode* node,
		const std::vector<DRenderObject*>& objects,
		const DAABoundingBox& boundingBox,
		EAxis axis,
		int RecursionDepth);
	void CreateBoundingBoxes(EAxis axis,
		const DAABoundingBox& boundingBox,
		const float lowestCostPosition,
		DAABoundingBox& leftAABB,
		DAABoundingBox& rightAABB);
	void CreateNode(DKDTreeNode* node, const std::vector<DRenderObject*>& objects, EAxis axis);
	void IntersectRecurse(DKDTreeNode* node, EAxis axis, const DVector3& start, const DVector3& end,
							   DRenderCache& RenderCache) const;
	DVector3 FindMedian(const std::vector<DRenderObject*>& objects);
	EAxis GetNextAxis(EAxis axis) const;;
	void MapObjectsRecurse( std::map<DRenderObject*, DRenderObject*>& ObjectMap, DKDTreeNode* node);

	DKDTreeNode* mRootNode;
	DAABoundingBox mRootBoundingBox;

};