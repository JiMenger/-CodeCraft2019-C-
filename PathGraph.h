#include<map>
#include<list>
#include<string>
#include<queue>
#include<memory>
#include<algorithm>
#include<cstdint>
#include<math.h>
#include<cmath>
#include <utility>
#include<set>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
typedef enum {UNDISCOVERED,DISCOVERED,VISITED} VStatus ;
typedef enum { UNSTART, START,WAIT,END,FINISH } CarStatus;
typedef enum { STRAIGHT, LEFT, RIGHT } CarDirection;
typedef enum {NotInEdgeGarage,InEdgeGarage }CarIsInEdgeGarage;
#define MIN(A,B) (A)>(B)?(B):(A)

struct Edge;
class car
{
public:
	car(int car_id, int from, int to, int speed, int plantime,int priority,int preset)
		:_car_id(car_id), _from(from), _to(to), _speed(speed), _plantime(plantime), _priority(priority), _preset(preset)
		,_start_time(-1),_end_time(-1) ,carStatus(UNSTART), undrive_distance(0), _channel(-1), current_edge_index(-1), lock_count(0), allowRePlanPath(true), isInGarage(NotInEdgeGarage)
		
	{

	}
	//���������ʻ·�����Ȩ�ر�
	Edge* getPlanPathMaxWeightEdge();

		//��ǰ��·������ٶ�
	int getCurrentRoadSpeed(void);
	//void recoverOldPath();
	//�¸���·�������ʻ�ٶ�
	int getNextRoadSpeed(void);
	Edge* getCurrentEdge();
	Edge* getNextEdge();
	//�Ƿ������ĵ�·
	bool isFinal();
	void recover();
	void save();
	CarStatus carStatus;
	int _car_id;//����Id
	int _end_time;//��������ʱ��
	int _from;//���ĸ�·�ڳ���
	int _to;//���ĸ�·��ֹͣ
	int _speed;//������ʻ����ٶ�
	int _plantime;//��С����ʱ��
	int _priority;//�Ƿ�����
	int _preset;//�Ƿ�ΪԤ�ó���

	int _start_time;//ʵ�ʳ���ʱ��
	int _channel;//���ڳ���
	int undrive_distance;//�����ڵ�ǰ��·��δ��ʻ�ľ���

	int lock_count;//����ʱ ÿ�εȴ�+1
	CarIsInEdgeGarage isInGarage;
	bool allowRePlanPath;//������ӵ�ǰʱ��Ƭδ�ܳɹ���·���ҳ�Ϊ��ֹ״̬�� ���������¹滮��·
	std::vector< Edge* > car_lock_road;//��ŵ��������ĵ�·




	std::vector< Edge* > car_new_plan_path;//��ų����µĹ滮·��
	//std::vector< Edge* > car_old_plan_path;//������¹滮֮ǰ��·��
	std::vector<Edge*> car_pass_path;//��ų����Ѿ���ʻ��·��
	int current_edge_index;
	//int old_edge_index;



	int last_channel;
	int last_undrive_distance;
	std::vector< Edge* >  last_car_new_plan_path;
	std::vector<Edge*> last_car_pass_path;//��ų����Ѿ���ʻ��·��
	int  last_current_edge_index;
	CarStatus last_carStatus;
	int last_start_time;

};
class road 
{
public:
	road(int road_id, int length, int speed, int channel, int from, int to, int isDuplex)
		:_road_id(road_id), _length(length), _speed(speed), _channel(channel), _from(from), _to(to), _isDuplex(isDuplex)
	{
		
	}

	int _road_id;//��·Id��
	int _length;//��·����
	int _speed;//��·���������ٶ�
	int _channel;//��·�ĳ�����
	int _from;//���ӵ�·��
	int _to;//���ӵ�·��
	int _isDuplex;//�Ƿ�˫��

};


class cross
{
	public:
	cross(int cross_id,int right_road_id,int buttom_road_id,int left_road_id,int top_road_id)
		:_cross_id(cross_id), _right_road_id(right_road_id), _buttom_road_id(buttom_road_id), _left_road_id(left_road_id), _top_road_id(top_road_id)
	{}
	int _cross_id;//·��Id
	int _right_road_id;//�ҵ�·Id
	int _buttom_road_id;//�µ�·Id
	int _left_road_id;//���·Id
	int _top_road_id;//�ϵ�·Id
	//std::list<car> notDriveCars;
}; 

//�߽ṹ
struct Edge
{
	//ע�������road�Ĳ�һ��
	int from;//�ߵ����
	int to;//�ߵ��յ��
	std::shared_ptr<road> data;//ÿ��·�е�����

	//std::set<CarDirection> car_direction_set;
	void updateSelfWeight()
	{

		double car_count = 0;
		double road_channel = data->_channel;
		double road_length = data->_length;

		for (int i = 0; i < carsInPerRoads.size(); i++)
		{
			car_count += carsInPerRoads[i].size();
		}

		weight = (1+ car_count / (road_channel*road_length)) * 1000;
	

	}

	void recover()
	{
		weight = last_weight;
		carsInPerRoads = last_carsInPerRoads;
	}
	void save()
	{

		last_weight = weight;
		last_carsInPerRoads = carsInPerRoads;
	}
	void sortCarsInGarage()
	{
		std::vector< std::shared_ptr<car>> vec_cars = std::vector< std::shared_ptr<car>>(ready_cars.begin(), ready_cars.end());

		//����ID
		std::stable_sort(vec_cars.begin(), vec_cars.end(),
			[](const std::shared_ptr<car>  &a, const std::shared_ptr<car>  &b)
		{return a->_car_id < b->_car_id; });

	    //����ʱ��
		std::stable_sort(vec_cars.begin(), vec_cars.end(),
			[](const std::shared_ptr<car>  &a, const std::shared_ptr<car>  &b)
		{return a->_start_time < b->_start_time; });
		//���ȼ�
		std::stable_sort(vec_cars.begin(), vec_cars.end(),
			[](const std::shared_ptr<car>  &a, const std::shared_ptr<car>  &b)
		{return a->_priority > b->_priority; });

		ready_cars = std::list< std::shared_ptr<car>>(vec_cars.begin(), vec_cars.end());
	}
	
	void putReadyCarInGarage(int time)
	{
		bool change = false;
		for (auto iter = garage_cars.begin(); iter != garage_cars.end(); ++iter)
		{

			if ((*iter)->_start_time <= time)
			{
				change = true;
				ready_cars.push_back((*iter));
				iter = garage_cars.erase(iter);
				--iter;
			}
			
		}
		if (change)
		{
			sortCarsInGarage();
		}
	}



	double weight;//�ߵ�Ȩ��
	std::vector<std::list<std::shared_ptr<car>>> carsInPerRoads;

	double last_weight;//�ϸ�ʱ��Ƭ �ߵ�Ȩ��
	std::vector<std::list<std::shared_ptr<car>>> last_carsInPerRoads;

	std::list<std::shared_ptr<car>> garage_cars;

	std::list<std::shared_ptr<car>> ready_cars;
	Edge(const std::shared_ptr<road>& d, int w) :data(d), weight(w)
	{
		carsInPerRoads = std::vector < std::list<std::shared_ptr<car>>>(data->_channel);
	}


};




//����ṹ
struct Vertex
{
	std::shared_ptr<cross> data;//ÿ��·���е�����
	VStatus status;
	int parent;//�������еĸ��ڵ㣬�����·���п��õ�
	//int priority;//������Դ���ÿ��·�ڵ�ӵ���̶ȣ��Ż���ʱ�����ʹ��
	double distance;//D�㷨�ľ���
	//Edge* pFirstEdgeOut;//�Ӷ�������ĵ�һ����
	//Edge* pFirstEdgeIn;//�Ӷ�������ĵ�һ����

//	std::vector<std::shared_ptr<Edge>> edgeInVec;
//	std::vector<std::shared_ptr<Edge>> edgeOutVec;
	std::vector<Edge*> edgeInVec;
	std::vector<Edge*> edgeOutVec;
	//int road_count;//���ӵ�·�ĸ���
	Vertex() {}
	Vertex(const std::shared_ptr<cross>& d) :data(d), parent(-1), status(UNDISCOVERED)//, pFirstEdgeOut(NULL), pFirstEdgeIn(NULL)
	{
	}
};



class PathGraph
{
public:
	PathGraph();//����
	~PathGraph();//����

	void reset();//���ж��㡢�ߵĸ�����Ϣ��λ
	std::shared_ptr<cross>& vertexData(int);//��ö�������

	int firstNbr(int);//�׸��ڽӶ��� 

	int nextNbr(int, int);//�����һ�ڽӶ������һ����
	VStatus& status(int);//��ö���״̬
	int& parent(int);//�ڱ������еĸ��ڵ�
	double& distance(int);//�ڱ������еľ���
	void insert(const std::shared_ptr<cross>&);//���붥��	
	void remove(const std::shared_ptr<cross>&);//ɾ������

	bool exists(int, int);//�ߣ�i��j���Ƿ����

	std::shared_ptr<road>& edgeData(int, int);//��ñ�(i,j)������
	Edge* getEdge(int, int);//��ñ�(i,j)
	Edge* getStraightEdge(std::shared_ptr<car>&); //��ȡֱ�е�edge
	Edge* getEdgeByRoadIdAndFromCrossId(int roadId,int from_crossId);//�õ�·Id��from��·�ڻ�ȡ��
	double weight(int, int);//��(i,j����Ȩ��

	void insert(const std::shared_ptr<road>&, int, int, int);//����һ����(i,j)

	void remove(int, int);//ɾ��һ����(i,j)
	void driveAllCarJustOnRoadToEndState(Edge* edge);//����ÿ����·�ϵĳ�

	void driveAllCarJustOnLaneToEndState(Edge* edge,int);//����ÿ����·�ϵĳ�

	void driveCarInGarage(Edge* edge,int,bool);//�������еĳ���·

	void init(std::vector<std::shared_ptr<road>>&, std::vector<std::shared_ptr<cross>>&, std::ifstream& ,std::vector<std::shared_ptr<car>>&);
	void start(std::vector<std::shared_ptr<car>>&);//��������ʼ��
	//�Ӷ���i��j�����·��
	void Dijkstra(int i, int j)
	{
		reset();
		std::priority_queue<P, std::vector<P>, std::greater<P>> Q;
		Q.push(P(0.0, i));
		v_map.at(i).distance = 0;
		while (!Q.empty())
		{
			int u = Q.top().second;
			Q.pop();

			for (auto iter = v_map.at(u).edgeOutVec.begin(); iter != v_map.at(u).edgeOutVec.end(); ++iter)
			{
				int v = (*iter)->to;
				int _weight = 0;
				if (distance(v) > distance(u) + weight(u, v))
				{
					distance(v) = distance(u) + weight(u, v);
					//printf("%d\n", weight(u, v));
					parent(v) = u;
					Q.push(P(distance(v), v));
				}

			}
		}
	}
	void planningPath(std::shared_ptr<car>& mycar);//Ϊ���滮·�� 

	 //���¹滮·��
	void rePlanningPath(std::shared_ptr<car>& mycar);


	//void planningPathTest(std::shared_ptr<car>& mycar);

	bool existWaitCars(std::vector<std::shared_ptr<car>>&);//�Ƿ���ڵȴ�����
	CarDirection getDirection(std::shared_ptr<car>);//��������¸���ʻת�䷽��
	void moveToNextRoad(std::shared_ptr<car>&,int);//ȥ��һ���ֵ�

	std::shared_ptr<car> getCarFromRoad(Edge*);

	//�����е���ͨ������·��ʻ
	//void driveNormalCarInGarage(std::vector<std::shared_ptr<car>>&,int time);

	void planningPathAndPutCarInGarage(std::vector<std::shared_ptr<car>>&,int);
	double getAverWeight();//��ȡ��·��ƽ��Ȩ��

	// ���еĳ�����Finish״̬ ˵���������
	bool allCarFinish(std::vector<std::shared_ptr<car>>&);

	 bool isConflict(std::shared_ptr<car>);//�Ƿ�����ͻ
	 void readAnswer(std::ifstream& in, std::vector<std::shared_ptr<car>>& car_list);
	 void saveAll(std::vector<std::shared_ptr<car>>&);
	 void recoverAll(std::vector<std::shared_ptr<car>>&); //���¹滮�� �ָ����е�·����Ϣ
	 void updateWeight();
private:
	//std::map<Vertex<T>,std::list<Edge<K> *>> V//���㼯��
	//std::vector<std::vector<Edge<K>* >> E;//�߼���
	//std::vector<Vertex> V;//�ɶ���ָ����
	std::map<int, Vertex> v_map;//�����ӳ��
	typedef std::pair<int, int> P; //first����̾��룬second�Ƕ���ı��
	//std::vector<Vertex> VIN;//ָ�򶥵�
	int n, e;//�������,������
};

void calculateTime(std::vector<std::shared_ptr<car>>&,int );