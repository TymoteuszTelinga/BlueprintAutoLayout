
#include "BlueprintLayout.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <set>
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
    std::unordered_map<uint32_t, std::vector<uint32_t>> parents;
    std::set<uint32_t> hasParent;
    for (const Edge& edge : m_Edges)
    {
        childrens[edge.From].push_back(edge.To);
        parents[edge.To].push_back(edge.From);
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

    //Layer to nodes list;
    std::map<int, std::vector<uint32_t>> layers;
    for (auto& [id, depth] : layer)
        layers[depth].push_back(id);

    //minimize egde crossings
    std::unordered_map<uint32_t, float> nodeOrder;
    for (auto& [depth, nodes] : layers)
        for (int i = 0; i < nodes.size(); i++)
            nodeOrder[nodes[i]] = i;

    for (int pass = 0; pass < 4; pass++) 
    {
        // Top-down
        for (auto& [depth, nodes] : layers)
        {
            for (uint32_t node : nodes)
            {
                auto& parentsIds = parents[node];
                if (parentsIds.empty()) 
                    continue;

                float avg = 0;
                for (uint32_t parent : parentsIds)
                {
                    avg += nodeOrder[parent];
                }
                nodeOrder[node] = avg / parentsIds.size();
            }
            
            std::stable_sort(nodes.begin(), nodes.end(), [&](int a, int b) { return nodeOrder[a] < nodeOrder[b]; });
            for (int i = 0; i < nodes.size(); i++)
                nodeOrder[nodes[i]] = i;
        }

        // Bottom-up
        for (auto it = layers.rbegin(); it != layers.rend(); ++it) 
        {
            auto& nodes = it->second;
            for (int node : nodes)
            {
                auto& childrensIds = childrens[node];
                if (childrensIds.empty()) 
                    continue;
                float avg = 0;
                for (int child : childrensIds)
                {
                    avg += nodeOrder[child];
                }
                nodeOrder[node] = avg / childrensIds.size();
            }

            std::stable_sort(nodes.begin(), nodes.end(), [&](int a, int b) { return nodeOrder[a] < nodeOrder[b]; });
            for (int i = 0; i < nodes.size(); i++)
                nodeOrder[nodes[i]] = i;
        }
    }

    for (auto& [depth, nodes] : layers) 
    {
        float totalWidth = nodes.size();
        float startX = -totalWidth / 2.f;

        for (int i = 0; i < nodes.size(); i++)
        {
            m_Nodes[nodes[i]].PosX = depth;
            m_Nodes[nodes[i]].PosY = startX + i;
        }
    }

    m_Nodes.erase(-1);
}

std::pair<float, float> BlueprintAutoLayout::GetGraphCenter() const
{
    float x = 0;
    float y = 0;
    float count = 0.f;

    for (auto [id, node] : m_Nodes)
    {
        x += m_Nodes.at(id).PosX;
        y += m_Nodes.at(id).PosY;
        count += 1.f;
    }
    return { x / count, y / count };
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
