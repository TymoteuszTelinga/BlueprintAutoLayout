
#include "BlueprintLayout.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <set>
//#include <stack>
#include <queue>

bool BlueprintAutoLayout::ImportGraph(const std::filesystem::path& importpath)
{
    YAML::Node data = YAML::LoadFile(importpath.string());

    m_Nodes.clear();
    m_Edges.clear();

    auto nodesData = data["Nodes"];
    for (const auto& nodeData : nodesData)
    {
        Node node;
        node.ID = nodeData["ID"].as<uint32_t>();
        node.Name = nodeData["Name"].as<std::string>();
        node.PosX = 0;
        node.PosY = 0;
        m_Nodes[node.ID] = node;
    }

    auto edgesData = data["Egdes"];
    for (const auto& edgeData : edgesData)
    {
        Edge edge;
        edge.From = edgeData["From"].as<uint32_t>();
        edge.To = edgeData["To"].as<uint32_t>();
        m_Edges.push_back(edge);
    }

    return true;
}

void BlueprintAutoLayout::CreateLayout()
{
    //node ID to childrens ID
    std::unordered_map<uint32_t, std::vector<uint32_t>> childrens;
    //std::unordered_map<uint32_t, std::vector<uint32_t>> parents;
    std::set<uint32_t> hasParent;
    for (const Edge& edge : m_Edges)
    {
        childrens[edge.From].push_back(edge.To);
        //parents[edge.To].push_back(edge.From);
        hasParent.insert(edge.To);
    }


    uint32_t entryNode = -1;//fake node
    for (auto [id, node] : m_Nodes)
    {
        if (!hasParent.count(id))
        {
            //curent ID is entry node
            childrens[entryNode].push_back(id);
        }
    }

    //bfs for assigning depth
    //node ID to depth
    std::unordered_map<uint32_t, int> layer;
    std::queue<uint32_t> bfsQueue;
    layer[entryNode] = 0;
    bfsQueue.push(entryNode);
    while (!bfsQueue.empty())
    {
        uint32_t parent = bfsQueue.front();
        bfsQueue.pop();
        for (uint32_t child : childrens[parent])
        {
            int childLayer = layer[parent] + 1;
            if (!layer.count(child) || layer[child] < childLayer)
            {
                layer[child] = childLayer;
                bfsQueue.push(child);
            }
        }
    }

    //layerID -> layerHeight
    std::unordered_map<int, int> layerHeight;
    for (auto [id, depth] : layer)
    {
        if (id == -1)
        {
            continue;
        }
        m_Nodes[id].PosX = depth;
        m_Nodes[id].PosY = layerHeight[depth];
        layerHeight[depth]++;
    }


    //Column to its height
    //std::unordered_map<int32_t, uint32_t> ColumnHeight;
    //std::set<uint32_t> VisitedNodes;
    //std::stack<uint32_t> NodesIDS;
    //for (auto [id, node] : m_Nodes)
    //{
    //    if (VisitedNodes.count(id))
    //    {
    //        continue;
    //    }

    //    for (const Edge& edge : m_Edges)
    //    {
    //        if (edge.From == id)
    //        {
    //            Nodes
    //        }
    //    }
    //}

    //for (auto [id, node] : m_Nodes)
    //{
    //    VisitedNodes.insert(node.ID);

    //    for (const Edge& edge : m_Edges)
    //    {
    //        if (edge.From == node.ID)
    //        {
    //            Node& target = m_Nodes[edge.To];
    //            if (VisitedNodes.find(target.ID) != VisitedNodes.end())
    //            {
    //                if (VisitedNodes.find(id) == VisitedNodes.end())
    //                {
    //                    node.PosX = target.PosX - 1;
    //                    node.PosY = ColumnHeight[node.PosX];
    //                    ColumnHeight[node.PosY]++;

    //                    VisitedNodes.insert(id);//lock parens
    //                    continue;
    //                }
    //            }

    //            target.PosX = node.PosX + 1;
    //            target.PosY = ColumnHeight[target.PosX];
    //            ColumnHeight[target.PosX]++;

    //            VisitedNodes.insert(target.ID); //lock target node position
    //        }
    //    }
    //}
}

void BlueprintAutoLayout::ExportGraph(const std::filesystem::path& exportPath)
{
    using namespace YAML;
    Emitter out;
    out << BeginMap;
    out << Key << "Nodes" << Value << BeginSeq;
    for (const auto& [id, node] : m_Nodes)
    {
        out << BeginMap;
        out << Key << "ID" << Value << node.ID;
        out << Key << "Name" << Value << node.Name;
        out << Key << "X" << Value << node.PosX;
        out << Key << "Y" << Value << node.PosY;
        out << EndMap;
    }
    out << EndSeq;
    out << Key << "Egdes" << Value << BeginSeq;
    for (const Edge& edge : m_Edges)
    {
        out << BeginMap;
        out << Key << "From" << Value << edge.From;
        out << Key << "To" << Value << edge.To;
        out << EndMap;
    }
    out << EndSeq;
    out << EndMap;

    std::ofstream file(exportPath);
    file << out.c_str();
}

void BlueprintAutoLayout::GenerateData()
{
    {
        Node n;
        n.ID = 1;
        n.Name = "Event BeginPlay";
        m_Nodes[n.ID] = n;
    }
    {
        Node n;
        n.ID = 2;
        n.Name = "Branch";
        m_Nodes[n.ID] = n;
    }
    {
        Node n;
        n.ID = 3;
        n.Name = "Play Sound (True)";
        m_Nodes[n.ID] = n;
    }
    {
        Node n;
        n.ID = 4;
        n.Name = "Spawn Actor (False)";
        m_Nodes[n.ID] = n;
    }

    {
        Edge e;
        e.From = 1;
        e.To = 2;
        m_Edges.push_back(e);
    }
    {
        Edge e;
        e.From = 2;
        e.To = 3;
        m_Edges.push_back(e);
    }
    {
        Edge e;
        e.From = 2;
        e.To = 4;
        m_Edges.push_back(e);
    }
}
