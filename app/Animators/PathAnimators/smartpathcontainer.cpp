#include "smartpathcontainer.h"

int firstSegmentNode(const int& nodeId, const QList<Node>& nodes) {
    if(nodeId < 0) return -1;
    if(nodeId >= nodes.count()) return -1;
    const Node * currNode = &nodes.at(nodeId);
    int smallestId = nodeId;
    int currId = nodeId;
    while(true) {
        //if(currNode->isDissolved() || currNode->isNormal())
        if(!currNode->hasPreviousNode()) return currId;
        const int prevId = currNode->getPrevNodeId();
        if(prevId == currId)
            RuntimeThrow("Node points to itself");
        if(prevId == smallestId) return smallestId;
        smallestId = qMin(prevId, smallestId);
        currNode = &nodes.at(prevId);
        currId = prevId;
    }
}

int lastSegmentNode(const int& nodeId, const QList<Node>& nodes) {
    if(nodeId < 0) return -1;
    if(nodeId >= nodes.count()) return -1;
    const Node * currNode = &nodes.at(nodeId);
    if(currNode->isDummy() || currNode->isMove()) return -1;
    int smallestId = nodeId;
    int currId = nodeId;
    while(true) {
        if(currNode->isDummy())
            RuntimeThrow("Dummy used as a next node(should be skipped)");
        //if(currNode->isDissolved() || currNode->isNormal())
        if(!currNode->hasNextNode()) return currId;
        const int nextId = currNode->getNextNodeId();
        if(nextId == currId)
            RuntimeThrow("Node points to itself");
        if(nextId == smallestId) return currId;
        smallestId = qMin(nextId, smallestId);
        currNode = &nodes.at(nextId);
        currId = nextId;
    }
}

int nodesInSameSagment(const int& node1Id,
                       const int& node2Id,
                       const QList<Node>& nodes) {
    const int firstSegment1 = firstSegmentNode(node1Id, nodes);
    const int firstSegment2 = firstSegmentNode(node2Id, nodes);
    return firstSegment1 && firstSegment1 == firstSegment2;
}

void reverseSegment(const int& nodeId, QList<Node>& nodes) {
    const int firstNodeId = firstSegmentNode(nodeId, nodes);
    if(firstNodeId == -1) return;
    Node& firstNode = nodes[firstNodeId];
    int nextSegId = firstNode.getNextNodeId();
    firstNode.switchPrevAndNext();
    while(const int currNodeId = nextSegId) {
        if(currNodeId == firstNodeId) break;
        Node& currNode = nodes[nextSegId];
        nextSegId = currNode.getNextNodeId();
        currNode.switchPrevAndNext();
    }
}

bool segmentClosed(const int& nodeId, const QList<Node>& nodes) {
    const int firstNodeId = firstSegmentNode(nodeId, nodes);
    if(firstNodeId == -1) return false;
    return nodes.at(firstNodeId).hasPreviousNode();
}

QList<QList<Node>> sortNodeListAccoringToConnetions(const QList<Node>& srcList) {
    QList<QList<Node>> result;
    QList<Node> segment;
    QList<int> srcIds;
    for(int i = 0; i < srcList.count(); i++) {
        srcIds << i;
    }
    while(!srcIds.isEmpty()) {
        const int firstSrcId = firstSegmentNode(srcIds.first(), srcList);
        if(firstSrcId == -1) {
            srcIds.removeFirst();
            continue;
        }
        const int firstResultId = segment.count();
        int nextSrcId = firstSrcId;
        bool first = true;
        while(true) {
            srcIds.removeOne(nextSrcId);
            const Node& srcNode = srcList.at(nextSrcId);
            Node newNode = srcNode;
            if(first) first = false;
            else newNode.setPrevNodeId(segment.count() - 1);

            if(newNode.hasNextNode())
                newNode.setNextNodeId(segment.count() + 1);

            segment << newNode;

            if(!srcNode.hasNextNode()) break;
            nextSrcId = srcNode.getNextNodeId();
            if(nextSrcId == firstSrcId) {
                segment[segment.count() - 1].setNextNodeId(firstResultId);
                segment[firstResultId].setPrevNodeId(segment.count() - 1);
                break;
            }
        }
        if(!segment.isEmpty()) {
            result << segment;
            segment.clear();
        }
    }
    if(!segment.isEmpty()) result << segment;
    return result;
}

void cubicTo(const Node& prevNode, const Node& nextNode,
             QList<qreal>& dissolvedTs, SkPath& result) {
    qCubicSegment2D seg(prevNode.fP1, prevNode.fC2,
                        nextNode.fC0, nextNode.fP1);
    qreal lastT = 0;
    for(const qreal& t : dissolvedTs) {
        const qreal mappedT = gMapTToFragment(lastT, 1, t);
        auto div = seg.dividedAtT(mappedT);
        const auto& first = div.first;
        result.cubicTo(qPointToSk(first.c1()),
                       qPointToSk(first.c2()),
                       qPointToSk(first.p1()));
        qDebug() << "Cubic to:" << first.p1();
        seg = div.second;
        lastT = t;
    }
    result.cubicTo(qPointToSk(seg.c1()),
                   qPointToSk(seg.c2()),
                   qPointToSk(seg.p1()));
    qDebug() << "Cubic to:" << seg.p1();
    dissolvedTs.clear();
}

SkPath nodesToSkPath(const QList<Node>& nodes) {
    const auto segments = sortNodeListAccoringToConnetions(nodes);
    SkPath result;

    const Node * firstNode = nullptr;
    const Node * prevNormalNode = nullptr;

    QList<qreal> dissolvedTs;

    for(const auto& segment : segments) {
        SkPath currPath;
        bool move = true;
        bool close = false;
        for(int i = 0; i < segment.count(); i++) {
            const Node& node = segment.at(i);
            if(node.isDummy()) continue;
            else if(node.isDissolved()) dissolvedTs << node.fT;
            else if(node.isMove()) RuntimeThrow("No MOVE node should have gotten here");
            else if(node.isNormal()) {
                if(move) {
                    firstNode = &node;
                    close = firstNode->hasPreviousNode();
                    currPath.moveTo(qPointToSk(node.fP1));
                    qDebug() << "Move to:" << node.fP1;
                    move = false;
                } else {
                    cubicTo(*prevNormalNode, node,
                            dissolvedTs, currPath);
                }
                prevNormalNode = &node;
            } else {
                RuntimeThrow("Unrecognized node type");
            }
        } // for each node
        if(close) {
            cubicTo(*prevNormalNode, *firstNode,
                    dissolvedTs, currPath);
            currPath.close();
        }
        result.addPath(currPath);
    }
    return result;
}

qCubicSegment2D segmentFromNodes(const Node& prevNode,
                                 const Node& nextNode) {
    return qCubicSegment2D(prevNode.fP1, prevNode.fC2,
                           nextNode.fC0, nextNode.fP1);
}

int prevNormalId(const int& nodeId, const QList<Node>& nodes) {
    const Node * currNode = &nodes.at(nodeId);
    while(currNode->hasPreviousNode()) {
        const int currId = currNode->getPrevNodeId();
        currNode = &nodes.at(currId);
        if(currNode->isNormal()) return currId;
    }
    return -1;
}

int nextNormalId(const int& nodeId, const QList<Node>& nodes) {
    const Node * currNode = &nodes.at(nodeId);
    while(currNode->hasNextNode()) {
        const int currId = currNode->getNextNodeId();
        currNode = &nodes.at(currId);
        if(currNode->isNormal()) return currId;
    }
    return -1;
}

int prevNonDummyId(const int& nodeId, const QList<Node>& nodes) {
    const Node * currNode = &nodes.at(nodeId);
    while(currNode->hasPreviousNode()) {
        const int currId = currNode->getPrevNodeId();
        currNode = &nodes.at(currId);
        if(!currNode->isDummy()) return currId;
    }
    return -1;
}

int nextNonDummyId(const int& nodeId, const QList<Node>& nodes) {
    const Node * currNode = &nodes.at(nodeId);
    while(currNode->hasNextNode()) {
        const int currId = currNode->getNextNodeId();
        currNode = &nodes.at(currId);
        if(!currNode->isDummy()) return currId;
    }
    return -1;
}

void promoteDissolvedNodeToNormal(const int& nodeId,
                                  Node& node,
                                  QList<Node>& nodes) {
    const int prevNormalIdV = prevNormalId(nodeId, nodes);
    const int nextNormalIdV = nextNormalId(nodeId, nodes);
    Node& prevNormal = nodes[prevNormalIdV];
    Node& nextNormal = nodes[nextNormalIdV];

    auto seg = segmentFromNodes(prevNormal, nextNormal);
    auto div = seg.dividedAtT(node.fT);
    const auto& first = div.first;
    const auto& second = div.second;
    prevNormal.fC2 = first.c1();
    node.fC0 = first.c2();
    node.fP1 = first.p1();
    node.fC2 = second.c1();
    node.setType(Node::NORMAL);
    nextNormal.fC0 = second.c2();
    for(int i = prevNormalIdV + 1; i < nodeId; i++) {
        Node& iNode = nodes[i];
        if(iNode.isDissolved()) {
            iNode.fT = gMapTToFragment(0, node.fT, iNode.fT);
        }
    }
    for(int i = nodeId + 1; i < nextNormalIdV; i++) {
        Node& iNode = nodes[i];
        if(iNode.isDissolved()) {
            iNode.fT = gMapTToFragment(node.fT, 1, iNode.fT);
        }
    }
}

void promoteDissolvedNodeToNormal(const int& nodeId,
                                  QList<Node>& nodes) {
    Node& node = nodes[nodeId];
    promoteDissolvedNodeToNormal(nodeId, node, nodes);
}

void splitNode(Node& node, const int& nodeId, QList<Node>& nodes) {
    Node newNode = node;
    if(node.isNormal()) {
        node.fC2 = node.fP1;
        newNode.fC0 = newNode.fP1;
        nodes.insert(nodeId + 1, newNode);
    } else if(node.isDissolved()) {
        nodes.insert(nodeId + 1, newNode);
    }
}

void splitNode(const int& nodeId, QList<Node>& nodes) {
    Node& node = nodes[nodeId];
    splitNode(node, nodeId, nodes);
}

void splitNodeAndDisconnect(const int& nodeId, QList<Node>& nodes) {
    Node& node = nodes[nodeId];
    if(!node.isNormal())
        RuntimeThrow("Can only disconnect normal nodes.");
    splitNode(node, nodeId, nodes);
    nodes.insert(nodeId + 1, Node(Node::MOVE));
}

bool shouldSplitThisNode(const Node& thisNode, const Node& neighNode) {
    const bool prevDiffers = thisNode.getPrevNodeId() !=
            neighNode.getPrevNodeId();
    const bool nextDiffers = thisNode.getNextNodeId() !=
            neighNode.getNextNodeId();
    // if node is normal
    if(thisNode.isNormal()) {
        // if node is in the middle(has both previous and next node)
        if(thisNode.hasNextNode() && thisNode.hasPreviousNode()) {
            // split node only if both nodes differ
            return prevDiffers && nextDiffers;
        }
    }
    // if node is not normal and in the middle
    // split if previous or next node differs
    return prevDiffers || nextDiffers;
}

void connectNodes(const int& node1Id, const int& node2Id,
                  QList<Node>& nodes) {
    Node& node1 = nodes[node1Id];
    Node& node2 = nodes[node2Id];
    if(!node1.hasNextNode() && !node2.hasPreviousNode()) {
        node1.setNextNodeId(node2Id);
        node2.setPrevNodeId(node1Id);
    } else if(!node1.hasPreviousNode() && !node2.hasNextNode()) {
        node1.setPrevNodeId(node2Id);
        node2.setNextNodeId(node1Id);
    } else if(!node1.hasPreviousNode() && !node2.hasPreviousNode()) {
        reverseSegment(node1Id, nodes);
        node1.setNextNodeId(node2Id);
        node2.setPrevNodeId(node1Id);
    } else if(!node1.hasNextNode() && !node2.hasNextNode()) {
        reverseSegment(node1Id, nodes);
        node1.setPrevNodeId(node2Id);
        node2.setNextNodeId(node1Id);
    } else {
        RuntimeThrow("Trying to connect nodes "
                     "that already have two connections");
    }
}

void connectTo(const int& toId, const int& nodeId,
               QList<Node>& nodes) {
    Node& toNode = nodes[toId];
    Node& node = nodes[nodeId];
    if(toNode.hasNextNode() && toNode.hasPreviousNode()) {
        const int nextNodeId = toNode.getNextNodeId();
        Node& nextNode = nodes[nextNodeId];
        node.setPrevNodeId(toId);
        node.setNextNodeId(nextNodeId);
        nextNode.setPrevNodeId(nodeId);
        toNode.setNextNodeId(nodeId);
    } else {
        connectNodes(toId, nodeId, nodes);
    }
}

void SmartPath::removeNodeFromList(const int &nodeId) {
    mNodes.removeAt(nodeId);
    for(int i = 0; i < mNodes.count(); i++) {
        Node& iNode = mNodes[i];
        iNode.shiftIdsGreaterThan(nodeId, -1);
    }
}

Node &SmartPath::insertNodeToList(const int &nodeId, const Node &node) {
    mNodes.insert(nodeId, node);
    for(int i = 0; i < mNodes.count(); i++) {
        Node& iNode = mNodes[i];
        iNode.shiftIdsGreaterThan(nodeId - 1, 1);
    }
    return mNodes[nodeId];
}

void SmartPath::actionRemoveNormalNode(const int &nodeId) {
    Node& node = mNodes[nodeId];
    if(!node.isNormal())
        RuntimeThrow("Invalid node type. "
                     "Only normal nodes can be removed.");

    Node * currNode = &node;
    while(currNode->hasPreviousNode()) {
        const int prevId = currNode->getPrevNodeId();
        currNode = &mNodes[prevId];
        if(currNode->isNormal() || currNode->isMove()) break;
        if(currNode->isDissolved()) currNode->fT *= 0.5;
    }
    currNode = &node;
    while(currNode->hasNextNode()) {
        const int nextId = currNode->getNextNodeId();
        currNode = &mNodes[nextId];
        if(currNode->isNormal() || currNode->isMove()) break;
        if(currNode->isDissolved()) currNode->fT = currNode->fT*0.5 + 0.5;
    }
    node.setType(Node::DUMMY);
    if(mPrev) mPrev->updateNodeTypeAfterNeighbourChanged(nodeId);
    if(mNext) mNext->updateNodeTypeAfterNeighbourChanged(nodeId);
    updateNodeTypeAfterNeighbourChanged(nodeId);
}

void SmartPath::actionAddFirstNode(const QPointF &c0,
                                   const QPointF &p1,
                                   const QPointF &c2) {
    const int insertId = mNodes.count();
    insertNodeToList(insertId, Node(c0, p1, c2));
    if(mPrev) mPrev->normalOrMoveNodeInsertedToNext(-1);
    if(mNext) mNext->normalOrMoveNodeInsertedToPrev(-1);
    insertNodeToList(insertId + 1, Node(Node::MOVE));
    connectNodes(insertId, insertId + 1, mNodes);
    if(mPrev) mPrev->normalOrMoveNodeInsertedToNext(insertId);
    if(mNext) mNext->normalOrMoveNodeInsertedToPrev(insertId);
}

void SmartPath::insertNodeAfter(const int &afterId,
                                const Node &nodeBlueprint) {
    Node& prevNode = mNodes[afterId];
    if(prevNode.isMove())
        RuntimeThrow("Unsupported previous node type");
    const int insertId = afterId + 1;
    Node &node = insertNodeToList(insertId, nodeBlueprint);
    const int nextNodeId = prevNode.getNextNodeId();
    node.setNextNodeId(nextNodeId);
    prevNode.setNextNodeId(insertId);
    node.setPrevNodeId(afterId);
    Node& nextNode = mNodes[nextNodeId];
    nextNode.setPrevNodeId(insertId);
    if(nodeBlueprint.isDissolved())
        promoteDissolvedNodeToNormal(insertId, node, mNodes);

    if(mPrev) mPrev->normalOrMoveNodeInsertedToNext(afterId);
    if(mNext) mNext->normalOrMoveNodeInsertedToPrev(afterId);
}

void SmartPath::actionInsertNormalNodeAfter(const int &afterId,
                                            const qreal &t) {
    insertNodeAfter(afterId, Node(t));
}

void SmartPath::actionAddNormalNodeAtEnd(const int &afterId,
                                         const QPointF &c0,
                                         const QPointF &p1,
                                         const QPointF &c2) {
    insertNodeAfter(afterId, Node(c0, p1, c2));
}

void SmartPath::actionPromoteDissolvedNodeToNormal(const int &nodeId) {
    promoteDissolvedNodeToNormal(nodeId, mNodes);
    if(mPrev) mPrev->updateNodeTypeAfterNeighbourChanged(nodeId);
    if(mNext) mNext->updateNodeTypeAfterNeighbourChanged(nodeId);
}

void SmartPath::actionDisconnectNodes(const int &node1Id, const int &node2Id) {
    Node& node1 = mNodes[node1Id];
    Node& node2 = mNodes[node2Id];
    int nextId;
    int prevId;
    if(node1.getNextNodeId() == node2Id) {
        nextId = node2Id;
        prevId = node1Id;
    } else if(node2.getNextNodeId() == node1Id) {
        nextId = node1Id;
        prevId = node2Id;
    } else {
        RuntimeThrow("Trying to disconnect not connected nodes");
    }
    {
        Node& nextNode = mNodes[nextId];
        nextNode.setPrevNodeId(-1);
    }
    const int moveInsertId = prevId + 1;
    {
        Node& moveNode = insertNodeToList(moveInsertId, Node(Node::MOVE));
        moveNode.setPrevNodeId(prevId);
    }
    {
        Node& prevNode = mNodes[prevId];
        prevNode.setNextNodeId(moveInsertId);
    }
    if(mPrev) mPrev->normalOrMoveNodeInsertedToNext(moveInsertId - 1);
    if(mNext) mNext->normalOrMoveNodeInsertedToPrev(moveInsertId - 1);
}

void SmartPath::actionConnectNodes(const int &node1Id,
                                   const int &node2Id) {
    const int moveNode1Id = lastSegmentNode(node1Id, mNodes);
    const int moveNode2Id = lastSegmentNode(node2Id, mNodes);
    {
        Node& node1 = mNodes[node1Id];
        if(node1.getNextNodeId() == moveNode1Id) {
            node1.setNextNodeId(-1);
        }
        Node& node2 = mNodes[node2Id];
        if(node2.getNextNodeId() == moveNode2Id) {
            node2.setNextNodeId(-1);
        }
    }
    connectNodes(node1Id, node2Id, mNodes);

    Node& moveNode1 = mNodes[moveNode1Id];
    if(!moveNode1.isMove())
        RuntimeThrow("Last node in not closed segment has to be MOVE node");
    moveNode1.setType(Node::DUMMY);
    if(mPrev) mPrev->updateNodeTypeAfterNeighbourChanged(moveNode1Id);
    if(mNext) mNext->updateNodeTypeAfterNeighbourChanged(moveNode1Id);
    if(moveNode1Id != moveNode2Id) { // connecting two segments
        Node& moveNode2 = mNodes[moveNode2Id];
        if(!moveNode2.isMove())
            RuntimeThrow("Last node in not closed segment has to be MOVE node");
        moveNode2.setType(Node::DUMMY);
        if(mPrev) mPrev->updateNodeTypeAfterNeighbourChanged(moveNode2Id);
        if(mNext) mNext->updateNodeTypeAfterNeighbourChanged(moveNode2Id);
    }
}

void SmartPath::normalOrMoveNodeInsertedToPrev(const int &targetNodeId) {
    dissolvedOrDummyNodeInsertedToPrev(targetNodeId);
    const int insertId = targetNodeId == -1 ? mNodes.count() : targetNodeId + 1;
    updateNodeTypeAfterNeighbourChanged(insertId);
}

void SmartPath::dissolvedOrDummyNodeInsertedToPrev(const int &targetNodeId) {
    const int insertId = targetNodeId == -1 ? mNodes.count() : targetNodeId + 1;
    insertNodeToList(insertId, Node());
    if(targetNodeId != -1) connectTo(targetNodeId, insertId, mNodes);
    if(mNext) mNext->dissolvedOrDummyNodeInsertedToPrev(targetNodeId);
}

void SmartPath::normalOrMoveNodeInsertedToNext(const int &targetNodeId) {
    dissolvedOrDummyNodeInsertedToNext(targetNodeId);
    const int insertId = targetNodeId == -1 ? mNodes.count() : targetNodeId + 1;
    updateNodeTypeAfterNeighbourChanged(insertId);
}

void SmartPath::dissolvedOrDummyNodeInsertedToNext(const int &targetNodeId) {
    const int insertId = targetNodeId == -1 ? mNodes.count() : targetNodeId + 1;
    insertNodeToList(insertId, Node());
    if(targetNodeId != -1) connectTo(targetNodeId, insertId, mNodes);
    if(mPrev) mPrev->dissolvedOrDummyNodeInsertedToNext(targetNodeId);
}

void SmartPath::removeNodeWithIdAndTellPrevToDoSame(const int &nodeId) {
    removeNodeFromList(nodeId);
    if(mPrev) mPrev->removeNodeWithIdAndTellPrevToDoSame(nodeId);
}

void SmartPath::removeNodeWithIdAndTellNextToDoSame(const int &nodeId) {
    removeNodeFromList(nodeId);
    if(mNext) mNext->removeNodeWithIdAndTellNextToDoSame(nodeId);
}

const Node::Type &SmartPath::nodeType(const int &nodeId) const {
    return mNodes.at(nodeId).getType();
}

bool SmartPath::isNodeNormal(const int &nodeId) const {
    return mNodes.at(nodeId).isNormal();
}

bool SmartPath::isNodeDissolved(const int &nodeId) const {
    return nodeType(nodeId) == Node::DISSOLVED;
}

bool SmartPath::isNodeDummy(const int &nodeId) const {
    return nodeType(nodeId) == Node::DUMMY;
}

QList<int> SmartPath::updateAllNodesTypeAfterNeighbourChanged() {
    QList<int> changed;
    for(int i = 0; i < mNodes.count(); i++) {
        if(updateNodeTypeAfterNeighbourChanged(i)) changed << i;
    }
    return changed;
}

qreal SmartPath::prevT(const int &nodeId) const {
    const int &prevId = prevNonDummyId(nodeId, mNodes);
    const Node& node = mNodes.at(prevId);
    if(node.isNormal()) return 0;
    return node.fT;
}

qreal SmartPath::nextT(const int &nodeId) const {
    const int &nextId = nextNonDummyId(nodeId, mNodes);
    const Node& node = mNodes.at(nextId);
    if(node.isNormal()) return 1;
    return node.fT;
}

bool SmartPath::updateNodeTypeAfterNeighbourChanged(const int &nodeId) {
    Node& node = mNodes[nodeId];
    if(node.isNormal() || node.isMove()) return false;
    Node::Type prevType = Node::DUMMY;
    Node::Type nextType = Node::DUMMY;
    int prevNextId = -1;
    int nextNextId = -1;
    if(mPrev) {
        const Node& prevNode = mPrev->getNodes().at(nodeId);
        prevType = prevNode.getType();
        prevNextId = prevNode.getNextNodeId();
    }
    if(mNext) {
        const Node& nextNode = mNext->getNodes().at(nodeId);
        nextType = nextNode.getType();
        nextNextId = nextNode.getNextNodeId();
    }
    if(prevType == Node::NORMAL || nextType == Node::NORMAL ||
            prevType == Node::MOVE || nextType == Node::MOVE ||
            (node.getNextNodeId() != nextNextId && nextType != Node::DUMMY) ||
            (node.getNextNodeId() != prevNextId && prevType != Node::DUMMY)) {
        if(node.getType() != Node::DISSOLVED) {
            node.fT = 0.5*(prevT(nodeId) + nextT(nodeId));
            node.setType(Node::DISSOLVED);
            return true;
        }
        return false;
    }
    if(node.getType() != Node::DUMMY) {
        node.setType(Node::DUMMY);
        return true;
    }
    return false;
}

void SmartPath::setPrev(SmartPath * const prev) {
    mPrev = prev;
    updateAllNodesTypeAfterNeighbourChanged();
}

void SmartPath::setNext(SmartPath * const next) {
    mNext = next;
    updateAllNodesTypeAfterNeighbourChanged();
}

const QList<Node> &SmartPath::getNodes() const {
    return mNodes;
}

SkPath SmartPath::getPathAt() const {
    return nodesToSkPath(mNodes);
}

SkPath SmartPath::interpolateWithNext(const qreal &nextWeight) const {
    if(!mNext) return getPathAt();
    SkPath result;
    getPathForNext().interpolate(mNext->getPathForPrev(),
                                 qrealToSkScalar(nextWeight),
                                 &result);
    return result;
}

SkPath SmartPath::interpolateWithPrev(const qreal &nextWeight) const {
    if(!mPrev) return getPathAt();
    SkPath result;
    getPathForPrev().interpolate(mPrev->getPathForNext(),
                                 qrealToSkScalar(nextWeight),
                                 &result);
    return result;
}

SmartPath::SmartPath() {}

SmartPath::SmartPath(const QList<Node> &nodes) :
    mNodes(nodes) {}

SkPath SmartPath::getPathForPrev() const {
    if(mPrev) return getPathFor(mPrev);
    return nodesToSkPath(mNodes);
}

SkPath SmartPath::getPathForNext() const {
    if(mNext) return getPathFor(mNext);
    return nodesToSkPath(mNodes);
}

SkPath SmartPath::getPathFor(SmartPath * const neighbour) const {
    const auto& neighNodes = neighbour->getNodes();
    QList<Node> result = mNodes;

    int iMax = neighNodes.count() - 1;
    if(result.count() - 1 != iMax)
        RuntimeThrow("Nodes count does not match");

    int iShift = 0;
    for(int i = 0; i <= iMax; i++) {
        const int resI = i + iShift;
        Node& resultNode = result[resI];
        const Node& neighbourNode = neighNodes.at(i);

        // Remove nodes if not needed
        if((neighbourNode.isDummy() || neighbourNode.isDissolved()) &&
                (resultNode.isDummy() || resultNode.isDissolved())) {
            iShift--;
            result.removeAt(resI);
        }

        // Create splits for connecting/disconnecting
        if(shouldSplitThisNode(resultNode, neighbourNode)) {
            if(resultNode.isDissolved()) {
                promoteDissolvedNodeToNormal(resI, result);
                splitNodeAndDisconnect(resI, result);
                iShift += 2;
            } else if(resultNode.isNormal()) {
                splitNode(resultNode, resI, result);
                iShift++;
            }
        }
    }
    return nodesToSkPath(result);
}