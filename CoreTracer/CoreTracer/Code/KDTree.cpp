#include "stdafx.h"
#include "KDTree.h"
#include "Vector3.h"
#include "RenderObject.h"
#include "Scene.h"
#include <algorithm>
#include "Log.h"

namespace
{
	const int MaxRecursionDepth=20;
};

void DKDTree::MapObjects( std::map<DRenderObject*, DRenderObject*>& ObjectMap )
{
	MapObjectsRecurse(ObjectMap, mRootNode);
}

void DKDTree::MapObjectsRecurse( std::map<DRenderObject*, DRenderObject*>& ObjectMap, DKDTreeNode* node)
{
	for (int i=0; i<(int)node->mObjects.size(); i++)
	{
//		node->mObject[i] = ObjectMap[mObject[i]];

		MapObjectsRecurse(ObjectMap, node->mLeft);
		MapObjectsRecurse(ObjectMap, node->mRight);
	}
}

void DKDTree::Initialise(const std::vector<DRenderObject*>& objects)
{
	std::vector<DRenderObject*>::const_iterator iter=objects.begin(), iterEnd=objects.end();
	DVector3 minv(10000,10000,10000);
	DVector3 maxv(-10000,-10000,-10000); // urgh
	while (iter!=iterEnd)
	{
		const DAABoundingBox& boundingBox = (*iter)->GetAABoundingBox();
		DVector3 bbMin = boundingBox.GetMinPoint();
		DVector3 bbMax = boundingBox.GetMaxPoint();
		if (bbMin[0]<minv[0]) minv[0] = bbMin[0];
		if (bbMin[1]<minv[1]) minv[1] = bbMin[1];
		if (bbMin[2]<minv[2]) minv[2] = bbMin[2];
		if (bbMax[0]>maxv[0]) maxv[0] = bbMax[0];
		if (bbMax[1]>maxv[1]) maxv[1] = bbMax[1];
		if (bbMax[2]>maxv[2]) maxv[2] = bbMax[2];
		++iter;
	}
	mRootBoundingBox.SetCentre((minv+maxv)*0.5f);
	mRootBoundingBox.SetExtents((maxv-minv)*0.5f);

//	InitialiseSimple(objects);
	InitialiseSAH(objects);
}

void DKDTree::InitialiseSAH(const std::vector<DRenderObject*>& objects)
{
	mRootNode = new DKDTreeNode;
	DAABoundingBox boundingBox = mRootBoundingBox;
	CreateNodeSAH(mRootNode, objects, boundingBox, XAxis, 0);
}

void DKDTree::CreateNodeSAH(DKDTreeNode* node,
	const std::vector<DRenderObject*>& objects,
	const DAABoundingBox& boundingBox,
	EAxis axis,
	int RecursionDepth)
{
	if (objects.size()<3 || RecursionDepth==MaxRecursionDepth)
	{
		node->mObjects = objects;
		return;
	}

	std::set<float> splitPositions;
	GetAllSplitPositions(objects, splitPositions, axis);
	std::set<float>::const_iterator splitIter = splitPositions.begin(),
		splitIterEnd = splitPositions.end();

	float lowestCost=100000000000.f, lowestCostPosition=-1;

	while (splitIter!=splitIterEnd)
	{
		float position = *splitIter;
		float cost = EvaluateCost(position, objects, boundingBox, axis);

		if (cost<lowestCost)
		{
			lowestCost = cost;
			lowestCostPosition = position;
		}

		++splitIter;
	}

	std::vector<DRenderObject*>::const_iterator iter=objects.begin(), iterEnd=objects.end();
	std::vector<DRenderObject*> leftList, rightList;
	for(;iter!=iterEnd; ++iter)
	{
		if ((*iter)->GetAABoundingBox().LessThanPlane(axis, lowestCostPosition))
		{
			leftList.push_back(*iter);
		}
		if ((*iter)->GetAABoundingBox().GreaterThanPlane(axis, lowestCostPosition))
		{
			rightList.push_back(*iter);
		}
	}

	node->mPoint = boundingBox.GetCentre();
	node->mPoint[axis] = lowestCostPosition;

	DAABoundingBox leftAABB;
	DAABoundingBox rightAABB;
	CreateBoundingBoxes(axis, boundingBox, lowestCostPosition, leftAABB, rightAABB);

	node->mLeft = new DKDTreeNode;
	node->mRight = new DKDTreeNode;
	EAxis nextAxis = GetNextAxis(axis);
	if (leftAABB.GetExtents()[axis]>0.f)
		CreateNodeSAH(node->mLeft, leftList, leftAABB, nextAxis, RecursionDepth+1);
	if (rightAABB.GetExtents()[axis]>0.f)
		CreateNodeSAH(node->mRight, rightList, rightAABB, nextAxis, RecursionDepth+1);
};

void DKDTree::CreateBoundingBoxes(EAxis axis,
	const DAABoundingBox& boundingBox,
	const float lowestCostPosition,
	DAABoundingBox& leftAABB,
	DAABoundingBox& rightAABB)
{
	DVector3 leftPos = boundingBox.GetMinPoint();
	DVector3 rightPos = boundingBox.GetMaxPoint();

	DVector3 leftAABBRightPos = rightPos;
	DVector3 rightAABBLeftPos = leftPos;
	rightAABBLeftPos[axis] = leftAABBRightPos[axis] = lowestCostPosition;

	leftAABB = DAABoundingBox(leftPos, leftAABBRightPos);
	rightAABB = DAABoundingBox(rightAABBLeftPos, rightPos);
}

float DKDTree::EvaluateCost(const float splitPosition,
	const std::vector<DRenderObject*>& objects,
	const DAABoundingBox& boundingBox,
	EAxis axis)
{
	std::vector<DRenderObject*>::const_iterator iter = objects.begin(),
		iterEnd = objects.end();

	int leftCount = 0, rightCount = 0;

	while (iter!=iterEnd)
	{
		if ((*iter)->GetAABoundingBox().LessThanPlane(axis, splitPosition))
		{
			leftCount++;
		}
		if ((*iter)->GetAABoundingBox().GreaterThanPlane(axis, splitPosition))
		{
			rightCount++;
		}
		++iter;
	}

	const float traversalCost = 0.001f;
	const float objectCost = 0.001f;

	float cost = traversalCost; // traversal cost

	switch (axis)
	{
	case XAxis:
		cost += (splitPosition - boundingBox.GetMinPoint()[0])
			* boundingBox.GetExtents()[1]*2
			* boundingBox.GetExtents()[2]*2
			* leftCount
			* objectCost;
		cost += (boundingBox.GetMaxPoint()[0] - splitPosition)
			* boundingBox.GetExtents()[1]*2
			* boundingBox.GetExtents()[2]*2
			* rightCount
			* objectCost;
		break;
	case YAxis:
		cost += (splitPosition - boundingBox.GetMinPoint()[1])
			* boundingBox.GetExtents()[0]*2
			* boundingBox.GetExtents()[2]*2
			* leftCount
			* objectCost;
		cost += (boundingBox.GetMaxPoint()[1] - splitPosition)
			* boundingBox.GetExtents()[0]*2
			* boundingBox.GetExtents()[2]*2
			* rightCount
			* objectCost;
		break;
	case ZAxis:
		cost += (splitPosition - boundingBox.GetMinPoint()[2])
			* boundingBox.GetExtents()[1]*2
			* boundingBox.GetExtents()[0]*2
			* leftCount
			* objectCost;
		cost += (boundingBox.GetMaxPoint()[2] - splitPosition)
			* boundingBox.GetExtents()[1]*2
			* boundingBox.GetExtents()[0]*2
			* rightCount
			* objectCost;
		break;
	};

	return cost;
};

void DKDTree::GetAllSplitPositions(const std::vector<DRenderObject*>& objects,
	std::set<float> &splitPositions,
	EAxis axis)
{
	std::vector<DRenderObject*>::const_iterator iter = objects.begin(),
		iterEnd = objects.end();

	while (iter!=iterEnd)
	{
		splitPositions.insert((*iter)->GetAABoundingBox().GetMinPoint()[axis]);
		splitPositions.insert((*iter)->GetAABoundingBox().GetMaxPoint()[axis]);
		++iter;
	}
};

void DKDTree::InitialiseSimple(const std::vector<DRenderObject*>& objects)
{
	mRootNode = new DKDTreeNode;
	CreateNode(mRootNode, objects, XAxis);
}

void DKDTree::CreateNode(DKDTreeNode* node, const std::vector<DRenderObject*>& objects, EAxis axis)
{
	if (objects.size()<2)
	{
		node->mObjects = objects;
		return;
	}

	node->mPoint = FindMedian(objects);
	std::vector<DRenderObject*>::const_iterator iter=objects.begin(), iterEnd=objects.end();
	std::vector<DRenderObject*> leftList, rightList;
	for(;iter!=iterEnd; ++iter)
	{
		if ((*iter)->GetAABoundingBox().LessThanPlane(axis, node->mPoint[axis]))
		{
			leftList.push_back(*iter);
		}
		if ((*iter)->GetAABoundingBox().GreaterThanPlane(axis, node->mPoint[axis]))
		{
			rightList.push_back(*iter);
		}
	}
	node->mLeft = new DKDTreeNode;
	node->mRight = new DKDTreeNode;
	EAxis nextAxis = GetNextAxis(axis);
	CreateNode(node->mLeft, leftList, nextAxis);
	CreateNode(node->mRight, rightList, nextAxis);
}

DVector3 DKDTree::FindMedian(const std::vector<DRenderObject*> &objects)
{
	DVector3 median = DVector3(0,0,0);
	std::vector<DRenderObject*>::const_iterator iter=objects.begin(), iterEnd=objects.end();
	while (iter!=iterEnd)
	{
		median += (*iter)->GetCentre();
		++iter;
	}
	median /= float(objects.size());

	return median;
}

struct hit
{
	hit(float dist, std::vector<DRenderObject*>::const_iterator i)
		: distance(dist),
		iter(i) {}
	float distance;
	std::vector<DRenderObject*>::const_iterator iter;
};

#define OBJCACHESIZE 8
class DRenderCache
{
public:
	DRenderCache(const DRayContext& rayContext, const int IgnoreObjectId, const int WithinObjectId)
		: mRayContext(rayContext),
		m_IgnoreObjectId(IgnoreObjectId),
		m_WithinObjectId(WithinObjectId),
		m_HitTestNum(0) {}

	bool Find(int objId)
	{
		for (int i=0; i<m_HitTestNum; i++)
		{
			if (objId==m_HitTested[i])
				return true;
		}
		return false;
	}

	void Add(int objId)
	{
		if (m_HitTestNum<OBJCACHESIZE)
		{
			m_HitTested[m_HitTestNum]=objId;
			m_HitTestNum++;
		}
	}
	const DRayContext& mRayContext;
	std::vector<hit> m_HitPoints;
	int m_HitTested[OBJCACHESIZE];
	int m_HitTestNum;
	int m_IgnoreObjectId;
	int m_WithinObjectId;
};

bool DKDTree::Intersect(const DRayContext& rRayContext, DCollisionResponse& out_Response, bool debuginfo) const
{
	DVector3 start, end;
	if (mRootBoundingBox.Intersects(rRayContext.m_Ray, start, end))
	{
		DRenderCache RenderCache(rRayContext, out_Response.mIgnoreObjectId, out_Response.m_WithinObjectId);
		IntersectRecurse(mRootNode, XAxis, start, end, RenderCache);

		// no hit
		if (RenderCache.m_HitPoints.size()==0)
		{
			return false;
		}

		// lets iterate over the hit points and find the nearest
		std::vector<hit>::iterator hitIter = RenderCache.m_HitPoints.begin(), hitEnd = RenderCache.m_HitPoints.end();
		std::vector<DRenderObject*>::const_iterator nearestObject=(*hitIter).iter;
		float nearestDistance = (*hitIter).distance;

		// we initialise to hitpoint[0] so increment before while
		hitIter++;

		// see if anything else is nearer
		while (hitIter!=hitEnd)
		{
			if ((*hitIter).distance < nearestDistance)
			{
				// woohoo, something nearer
				nearestObject = (*hitIter).iter;
				nearestDistance = (*hitIter).distance;
			}
			hitIter++;
		}

		if (debuginfo)
		{
			std::string kdDebug;
			std::stringstream ss;
			ss<<"KDTree intersect, obj intersected : ";
			for (unsigned int i=0; i<RenderCache.m_HitPoints.size(); i++)
			{
				ss<<(*(RenderCache.m_HitPoints[i].iter))->GetObjectId()<<", ";
			}
			ss<<"\r\nObj Tested : ";
			for (int i=0; i<OBJCACHESIZE; i++)
			{
				ss<<RenderCache.m_HitTested[i]<<", ";
			}
			ss<<"\r\nObj Tested Total: "<<RenderCache.m_HitTestNum;
			ss<<"\r\nRay start : "<<rRayContext.m_Ray.GetStart()[0]<<", "<<rRayContext.m_Ray.GetStart()[1]<<", "<<rRayContext.m_Ray.GetStart()[2];
			ss<<"\r\nRay direction : "<<rRayContext.m_Ray.GetDirection()[0]<<", "<<rRayContext.m_Ray.GetDirection()[1]<<", "<<rRayContext.m_Ray.GetDirection()[2];
			ss<<"\r\nRay recursionRemaining : "<<rRayContext.m_RecursionRemaining;
			ss<<"\r\nRay AirRefractionIndex : "<<rRayContext.AirRefractionIndex;
			ss<<"\r\nRay RefractiveIndex : "<<rRayContext.m_RefractiveIndex;
			ss<<"\r\nRay m_RequestFlags : "<<rRayContext.m_RequestFlags;
			ss<<"\r\n";
			kdDebug = ss.str();
			Log().Print(kdDebug.c_str());
		}

		// recalculate interestion against the nearest object, calculating the colour + lighting etc.
		(*nearestObject)->Intersect(rRayContext, out_Response);

		return true;
	}
	return false;
}

void DKDTree::IntersectRecurse(DKDTreeNode* node, EAxis axis, const DVector3& start, const DVector3& end,
							   DRenderCache& RenderCache) const
{
	mRecursions++;
	const DVector3& direction = RenderCache.mRayContext.m_Ray.GetDirection();

	// if we're at a leaf
	if (node->mLeft==NULL)
	{
		std::vector<DRenderObject*>::iterator iter=node->mObjects.begin(), iterEnd=node->mObjects.end();
		while (iter!=iterEnd)
		{
			DCollisionResponse tempResponse; // this should be fine as pixelindex is only used to guarantee sequential random numbers...
			tempResponse.mIgnoreObjectId = RenderCache.m_IgnoreObjectId;
			tempResponse.m_WithinObjectId = RenderCache.m_WithinObjectId;
			if (!RenderCache.Find((*iter)->GetObjectId()))
			{
				if (((RenderCache.mRayContext.m_RequestFlags!=RequestLighting)||(!(*iter)->GetIgnoreWhileLighting())))
				{
					mIntersections++;
					if ((*iter)->Intersect(DRayContext(NULL, RenderCache.mRayContext.m_Ray, RequestDistance, 0, RenderCache.mRayContext.m_RefractiveIndex, RenderCache.mRayContext.mPixelIndex), tempResponse)
						&& (*iter)->GetObjectId()!=RenderCache.m_IgnoreObjectId)
					{
						RenderCache.m_HitPoints.push_back(hit(tempResponse.mDistance, iter));
					}
				}
				RenderCache.Add((*iter)->GetObjectId());
			}
			iter++;
		}
		return;
	}

	if ((start[axis]<node->mPoint[axis]) != (end[axis]<node->mPoint[axis]))
	{
		// they split the cutting plane, so find the intersection point
		float t = (node->mPoint[axis]-start[axis])/direction[axis];
		DVector3 split = start + direction*t;
		if (start[axis]<node->mPoint[axis])
		{
			IntersectRecurse(node->mLeft, GetNextAxis(axis), start, split, RenderCache);
			IntersectRecurse(node->mRight, GetNextAxis(axis), split, end, RenderCache);
		}
		else
		{
			IntersectRecurse(node->mLeft, GetNextAxis(axis), split, end, RenderCache);
			IntersectRecurse(node->mRight, GetNextAxis(axis), start, split, RenderCache);
		}
	}
	else if (start[axis]<node->mPoint[axis]) // both are <
	{
		IntersectRecurse(node->mLeft, GetNextAxis(axis), start, end, RenderCache);
	}
	else
	{
		IntersectRecurse(node->mRight, GetNextAxis(axis), start, end, RenderCache);
	}
}

EAxis DKDTree::GetNextAxis(EAxis axis) const
{
	switch (axis)
	{
	case XAxis: return YAxis;
	case YAxis: return ZAxis;
	case ZAxis: return XAxis;
	};
	return XAxis;
}
