#include<cstdio>
#include<cstring>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<vector>
#include<set>
#include<map>
#include<queue>
#include<algorithm>
#include<ctime>
#include "flatten_hash_multimap.h"
#include<sstream>
using namespace std;

clock_t ct;
int cnt, tree_width = 0;
vector<vector<pair<unsigned int, unsigned int>>> Edge;
vector<int> D;
int max_deg = 0;
int n_v = 0;	

typedef tuple<unsigned int, unsigned int,vector<unsigned int>> tii; // tuple of (vertex, dist, mp)

// typedef 1000000 num;

// initialize graph data
struct Graph{
	int n, m;
	
	Graph(){
		n = m = 0;
		D.clear();
		Edge.clear();
	}

	Graph(char *file){
		Graph();
		FILE *fin = fopen(file, "r");
		fscanf(fin, "%d", &n);
		fscanf(fin, "%d", &m);
		n_v = n;
		printf("n m: %d %d\n", n, m);

		Edge.resize(n+1);
		D.resize(n+1);

		int x, y, z = 0;
		for (unsigned long i = 0; i < m; ++i) {
			fscanf(fin, "%d%d%d", &x, &y, &z);
			Edge[x].emplace_back(y, z);
			Edge[y].emplace_back(x, z);
			++D[x];
			++D[y];
		}
		for (int v = 1; v <= n; ++v) if(max_deg < D[v]) max_deg = D[v];
	}
};

struct Node{
	int uniqueVertex;
	koala::my_openadd_hashmap<unsigned int, vector<unsigned int>> edges; // contains: vert & VL & middle vertex set
	int height;
	int pa;
	Node(){
		edges.clear();
		pa = -1;
		uniqueVertex = -1;
		height = 0;
	}
};

class Tree_Decomposition{
public: 
	FILE *fout, *fin;
	Graph G; 
	Tree_Decomposition(){
	}
	vector<int> ord;
	int heightMax;

	vector<int> vertexOrder;

	vector<Node> tnodes;
	int vorder = 0;
	
	void reduction(){
		vector<pair<int,int>> vPosition(n_v+1); //(degree,idx)
		int degree2nodeQ_size =0;

		vector<int> degree; degree.clear();
		vector<vector<int>> degree2nodeQ(max_deg + 1,degree); //degree->a list of vertex of this degree

		for (int v = 1; v <= n_v; ++v) {
			vPosition[v] = make_pair(D[v], degree2nodeQ[D[v]].size());
			degree2nodeQ[D[v]].push_back(v);
		}
		
		vector<koala::my_openadd_hashmap<unsigned int, vector<unsigned int>>> shortcuts(n_v + 1);
		for (int s = 1; s <= n_v; ++s) {
			for (unsigned int i = 0; i < D[s]; ++i) {
				shortcuts[s].insert(Edge[s][i].first, Edge[s][i].second, {Edge[s][i].first});
			}
		}

		vertexOrder.resize(n_v + 1, -1);
		ord.clear();
		int mindegree = 0;
		tnodes.resize(n_v + 1);

		int cnt_width = 0;
		while (true) {
			int cnt =0;
			while (degree2nodeQ[mindegree].size() == 0) {
				cnt += degree2nodeQ[mindegree].size();
				++mindegree;
				if (mindegree == degree2nodeQ.size() && cnt==0) break;
			}
			if (mindegree == degree2nodeQ.size() && cnt==0) break;

			unsigned int vid = degree2nodeQ[mindegree].back();
			degree2nodeQ[mindegree].pop_back();
			
			ord.push_back(vid);
			// cout<<vid<<": ";
			vertexOrder[vid] = vorder++;
			
			koala::my_openadd_hashmap<unsigned int, vector<unsigned int>> v = shortcuts[vid];
			vector<unsigned int> valid_neighbor_index;
			cnt_width = 0;
			
			for (unsigned int i = v.iterator(); v.has_next(i); v.next(i)) {
				if (vertexOrder[get<0>(v.get_with_idx(i))] == -1) {
					valid_neighbor_index.push_back(i);
					tnodes[vid].edges.insert(v.get_with_idx(i));
					++cnt_width;
					int vec = get<0>(v.get_with_idx(i)), di = get<1>(v.get_with_idx(i));
					vector<unsigned int> mvs = get<2>(v.get_with_idx(i));
					
				}
			}
			
			vector<int> neighbor_degree_increase_cnt(cnt_width, -1);
			
			for (unsigned int ii = 0; ii < cnt_width; ++ii) {
				for (unsigned int jj = ii + 1; jj < cnt_width; ++jj) {
					unsigned int i = valid_neighbor_index[ii], j = valid_neighbor_index[jj];
					auto vidi = get<0>(v.get_with_idx(i)), vidj = get<0>(v.get_with_idx(j));
					unsigned int d = get<1>(v.get_with_idx(i)) + get<1>(v.get_with_idx(j));
					if (shortcuts[vidi].find(vidj) != -1) {//exist, update with min value
						
						if(shortcuts[vidi](vidj) < d ){
							continue;
						}else if(shortcuts[vidi](vidj) > d){
							shortcuts[vidj].update(vidi, d, {vid});
							shortcuts[vidi].update(vidj, d, {vid});
						}else{
							shortcuts[vidi][vidj].emplace_back(vid);
							shortcuts[vidj][vidi].emplace_back(vid);
						}
					} else {//doesn't exist, add shortcut
						shortcuts[vidj].insert(vidi, d, {vid});
						shortcuts[vidi].insert(vidj, d, {vid});
						++neighbor_degree_increase_cnt[ii];
						++neighbor_degree_increase_cnt[jj];
					}
				}
			}
			
			for (unsigned int i = 0; i < cnt_width; ++i) {
				if (neighbor_degree_increase_cnt[i] != 0) {
					unsigned int &vid = get<0>(v.get_with_idx(valid_neighbor_index[i]));
					pair<int, int> &p = vPosition[vid];
					//swap and delete 
					if (degree2nodeQ[p.first][p.second] != degree2nodeQ[p.first].back()){
						degree2nodeQ[p.first][p.second] = degree2nodeQ[p.first].back();
						vPosition[degree2nodeQ[p.first].back()].second = p.second;
					}
					degree2nodeQ[p.first].pop_back();
					//place in a new position
					p.first += neighbor_degree_increase_cnt[i];
					if(p.first >= degree2nodeQ.size()) degree2nodeQ.resize(p.first + 1);
					p.second = degree2nodeQ[p.first].size();
					degree2nodeQ[p.first].push_back(vid);
					if(p.first < mindegree) mindegree = p.first;
				}
			}
		}
	}

	

	// ord: order -> original id
	// vertexOrder: original id -> order

	

	void kvc(){	
		int trlen = n_v-1;
		int order = n_v+1;
		int nearest=0, neighbor=0; 
		tnodes[trlen].pa = -1;
		tnodes[trlen].height = 1;
		--trlen;

		//按照 order 在取vertex 
		for (; trlen >= 0; --trlen){
			int x = ord[trlen];
			order = n_v+1;
			auto& edges = tnodes[x].edges;
			int ki = 0, kj = 0;
			for (unsigned int i = edges.iterator(); edges.has_next(i); edges.next(i)) {
				auto &neib1 = edges.get_with_idx(i);
				unsigned int  vid1 = get<0>(neib1);
				for (unsigned int j = i; edges.has_next(j); edges.next(j)) {
					if(i==j) continue;
					auto &neib2 = edges.get_with_idx(j);
					unsigned int d = dist(get<0>(neib1), get<0>(neib2));
					if (get<1>(neib1) > ((get<1>(neib2)+ d))){
						get<1>(neib1) = get<1>(neib2) + d;
						get<2>(neib1) = {get<0>(neib2)};
					} 
					else if(get<1>(neib1) == ((get<1>(neib2)+ d))){
						get<2>(neib1).emplace_back(get<0>(neib2));
					}
					else if (get<1>(neib2) > ((get<1>(neib1) + d))){
						get<1>(neib2) = get<1>(neib1) + d;
						get<2>(neib2) = {get<0>(neib1)};
					} 
					else if (get<1>(neib2) == ((get<1>(neib1) + d))){
						get<2>(neib2).emplace_back(get<0>(neib1));
					}
				}
				neighbor = get<0>(neib1);
				if (vertexOrder[neighbor] < order){
					nearest = neighbor;
					order = vertexOrder[neighbor];
				}
			}
			tnodes[x].pa = nearest;
			tnodes[x].height = tnodes[tnodes[x].pa].height + 1;
			for (unsigned int i = edges.iterator(); edges.has_next(i); edges.next(i)) {
				// cout<<"("<<edges.get_with_idx(i).first<<", "<<edges.get_with_idx(i).second<<")";
				// cout<<"("<<get<0>(edges.get_with_idx(i))<<", "<<get<1>(edges.get_with_idx(i))<< " vertices: ";
				// for(auto vec: get<2>(edges.get_with_idx(i))) cout<<vec<<", "; 
				// cout<<")"<<endl;
			}
			// cout<<endl;
		}
	}


	int qLCA(int s, int t) {
		int sAncestor = s, tAncestor = t;
		while (true) {
			if (tnodes[sAncestor].height > tnodes[tAncestor].height) {
				if (tAncestor == tnodes[sAncestor].pa) return tAncestor;
				sAncestor = tnodes[sAncestor].pa;
			} else if (tnodes[sAncestor].height < tnodes[tAncestor].height) {
				if (sAncestor == tnodes[tAncestor].pa) return sAncestor;
				tAncestor = tnodes[tAncestor].pa;
			} else {
				if (tnodes[sAncestor].pa == -1 || sAncestor == tAncestor) return sAncestor;
				sAncestor = tnodes[sAncestor].pa;
				tAncestor = tnodes[tAncestor].pa;
			}
		}
	}
	
	void BU_qcomputing(unsigned int bottom, unsigned int up, koala::my_openadd_hashmap<unsigned int,vector<unsigned int>> & distmap) {
		vector<tuple<unsigned int, unsigned int,vector<unsigned int>>> result;
		distmap = tnodes[bottom].edges;
		if (bottom != up) {
			unsigned int anc = tnodes[bottom].pa, anc_c = bottom;
			while (anc_c != up) {
				vector<tuple<unsigned int, unsigned int, vector<unsigned int>>> minus_set;
				auto &edges = tnodes[anc].edges;
				for (unsigned int i = edges.iterator(); edges.has_next(i); edges.next(i)) {
					auto &e = edges.get_with_idx(i);
					if (distmap.find(get<0>(e)) == -1){
						minus_set.push_back({get<0>(e), UINT32_MAX,{}});
					} 
				}
				for (auto &p:minus_set) {
					auto &cedges = tnodes[anc_c].edges;
					for (unsigned int i = cedges.iterator(); cedges.has_next(i); cedges.next(i)) {
						auto &e = cedges.get_with_idx(i);
						
						if ((distmap(get<0>(e)) + dist(get<0>(e), get<0>(p))) < get<1>(p)) {
							get<1>(p) = distmap(get<0>(e)) + dist(get<0>(e), get<0>(p));
							get<2>(p) = {get<0>(e)};
						}else if((distmap(get<0>(e)) + dist(get<0>(e), get<0>(p))) == get<1>(p)) {
							get<2>(p).emplace_back(get<0>(e));
						}
					}
				}
				for (auto &p : minus_set) distmap.insert(p);
				anc_c = anc;
				anc = tnodes[anc].pa; //??
			}
		} else {
			distmap.insert(up, 0, {up});
		}
	}
	
	inline unsigned int dist(int s, int t) {
		if (s == t) return 0;
		else if (vertexOrder[s] < vertexOrder[t])
			swap(s, t);
		return tnodes[t].edges.getDist(s);
	}

	vector<vector<unsigned int>> sub_g;
	vector<unsigned int> visited_u;
	vector<unsigned int> visited_v;
	vector<int>visited_s;
	vector<int>visited_t;
	int cnt;

	void subgraph(unsigned int s, unsigned int t, vector<vector<unsigned int>> & sub_g, vector<int> & visited, vector<unsigned int> & visited_e, koala::my_openadd_hashmap<unsigned int,vector<unsigned int>>& distmap){
		if(s==t or visited[t]==cnt) return;
		
		if (tnodes[s].edges.find(t)==-1){
			auto mp = distmap.getMP(t);
			for(auto& w: mp){
				edge_explore(t,w,visited_e,sub_g);
				subgraph(s,w,sub_g,visited,visited_e,distmap);
			} 
		}else edge_explore(s,t,visited_e,sub_g);
	}
	
	void edge_explore(unsigned int s, unsigned int t, vector<unsigned int> & visited_e, vector<vector<unsigned int>> & sub_g){	
		if (s == t) return;
		else if (vertexOrder[s] < vertexOrder[t]) swap(s, t);
		// t 是底端的 tree node
		if(visited_u[s]== cnt and visited_v[t] == cnt) return;

		// cout<<"come explore"<<" s: "<<s<<" t: "<<t<<endl;

		visited_u[s] = cnt;
		visited_v[t] = cnt;
		auto mp = tnodes[t].edges.getMP(s);

		for(auto& w : mp){
			// cout<<"w: "<<w<<" t: "<<t<<endl;
			if((s == w) ){
				// cout<<"s: "<<s<<" w: "<<w<<"dis: "<<dist(t,w)<<endl;
				sub_g.push_back({t,w,dist(t,w)});
			}else{
				edge_explore(s,w,visited_e,sub_g);
				edge_explore(t,w,visited_e,sub_g);
			}
		}
	}
	

	// pair<unsigned int, vector<unsigned int>> 
	unsigned int query(int s, int t) {
		// cout<<"s: "<<s<<" t: "<<t<<endl;
		++cnt;
		unsigned int lca = qLCA(s, t);
		// cout<<"cnt: "<<cnt<<endl;
		koala::my_openadd_hashmap<unsigned int,vector<unsigned int>> distmap_s;
		BU_qcomputing(s, lca, distmap_s);
		
		koala::my_openadd_hashmap<unsigned int,vector<unsigned int>> distmap_t;
		BU_qcomputing(t, lca, distmap_t);

		unsigned int mindist = UINT32_MAX;
		vector<unsigned int> pv = {}; //path vertices

		
		tuple<unsigned int, unsigned int,vector<unsigned int>> dm_s;
		tuple<unsigned int, unsigned int,vector<unsigned int>> dm_t;

		auto &cedges = tnodes[lca].edges;
		for (unsigned int i = cedges.iterator(); cedges.has_next(i); cedges.next(i)) {
			auto &e = cedges.get_with_idx(i);
			dm_s = distmap_s.getDistMSet(get<0>(e));
			dm_t = distmap_t.getDistMSet(get<0>(e));
			if (mindist > (get<1>(dm_s) + get<1>(dm_t))){
				mindist = get<1>(dm_s) + get<1>(dm_t);
				pv = {get<0>(e)};
			}else if(mindist == (get<1>(dm_s) + get<1>(dm_t))){
				pv.emplace_back(get<0>(e));
			}
		}
		dm_s = distmap_s.getDistMSet(lca);
		dm_t = distmap_t.getDistMSet(lca);
		if (mindist > (get<1>(dm_s) + get<1>(dm_t))){
			mindist = get<1>(dm_s) + get<1>(dm_t);
			pv = {lca};
		}else if(mindist == (get<1>(dm_s) + get<1>(dm_t))){
			pv.emplace_back(lca);
		}
		// if(mindist == UINT32_MAX) return UINT32_MAX;

		sub_g = {};
		for(auto com:pv){
			// cout<<"s: "<<s<<" t: "<<t<<" com: "<<com<<endl;
			subgraph(s,com,sub_g, visited_s, visited_u, distmap_s);
			subgraph(t,com,sub_g, visited_t, visited_v, distmap_t);
		}
		
		return mindist;
	}

	void query_init(int n){
		sub_g.clear();
		visited_u.resize(n, -1);
		visited_v.resize(n, -1);
		visited_s.resize(n, -1);
		visited_t.resize(n, -1);
		cnt = 0;
	}

};



vector<pair<int,int>> queries;

void read_queryVertex(string queryFile){
	   // 打开文件
    std::ifstream inputFile(queryFile);

    // 检查文件是否成功打开
    if (!inputFile.is_open()) {
        std::cerr << "无法打开文件！" << std::endl;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        // 使用字符串流来解析每一行的数据
        std::istringstream iss(line);
        int data1, data2;

        if (!(iss >> data1 >> data2)) {
            std::cerr << "读取数据失败！" << std::endl;
            continue;
        }

	queries.push_back({data1,data2});
    }
    // 关闭文件
    inputFile.close();
}

// floyd deal with vertices which is deleted
int main(int argc, char *argv[])
{
	srand(time(0));
	int operation = 1;
	if (operation == 1){ // index 
		string filest;
		char *file, *queryfile;
		// int i;
		file = argv[1];
		cout << "file: " << file << endl;
		// fileout = argv[2];
		Tree_Decomposition td;

		// read graph data
		td.G = Graph(file);
		// cout<<"finish graph"<<endl;
		
		clock_t start = clock();

		// reduction
		td.reduction(); 
		// top_down 
		td.kvc();
		cout << "MakeIndex time: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
		td.query_init(td.G.n);

		queryfile = argv[2];

		td.query_init(td.G.n);

		int qu_cnt = stoi(argv[3]);

		read_queryVertex(queryfile);

		start = clock();

		for(auto & q:queries){
			td.query(q.first, q.second);
		}
	
		cout << "ave query time: " << (double)(clock() - start) / (CLOCKS_PER_SEC * qu_cnt) << endl;
		

	}

}
