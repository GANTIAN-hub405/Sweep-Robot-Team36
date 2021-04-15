
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <stack>
#include <cmath>

#define KDtreeSize 1000

#define UL unsigned long

using namespace std;

struct coordinate
{
	double x = 0;
	double y = 0;
	UL index = 0;
};

struct TreeNode
{
	struct coordinate dom_elt;
	UL split = 0;
	struct TreeNode* left = nullptr;
	struct TreeNode* right = nullptr;
};

bool cmp1(const coordinate& a, const coordinate& b) {
	return a.x < b.x;
}

bool cmp2(const coordinate& a, const coordinate& b) {
	return a.y < b.y;
}

bool equal(const coordinate& a, const coordinate& b) {
	return (a.x == b.x && a.y == b.y);
}

void ChooseSplit(coordinate exm_set[], UL size, UL &split, coordinate &SplitChoice) {
	/*
	1. ����ÿ��ά��(x,y)�ķ���,�Ӿ�����󷽲��ά�ȿ�ʼ�з֣���x����
	2. ����x����Ĳ�������ֵ����Ϊ��ʼ�ڵ㣻
	*/

	double tmp1, tmp2;
	tmp1 = tmp2 = 0;
	for (int i = 0; i < size; ++i)
	{
		tmp1 += 1.0 / (double)size * exm_set[i].x * exm_set[i].x;
		tmp2 += 1.0 / (double)size * exm_set[i].x;
	}
	double v1 = tmp1 - tmp2 * tmp2;  //compute variance on the x dimension

	tmp1 = tmp2 = 0;
	for (int i = 0; i < size; ++i)
	{
		tmp1 += 1.0 / (double)size * exm_set[i].y * exm_set[i].y;
		tmp2 += 1.0 / (double)size * exm_set[i].y;
	}
	double v2 = tmp1 - tmp2 * tmp2;  //compute variance on the y dimension

	split = v1 > v2 ? 0 : 1; //set the split dimension

							 //����
	if (split == 0)
	{
		sort(exm_set, exm_set + size, cmp1);
	}
	else {
		sort(exm_set, exm_set + size, cmp2);
	}

	//set the split point value:��ֵ
	SplitChoice.x = exm_set[size / 2].x;
	SplitChoice.y = exm_set[size / 2].y;
}

//�ݹ鴴��kdtree
TreeNode* build_kdtree(coordinate exm_set[], UL size, TreeNode* T) {
	//call function ChooseSplit to choose the split dimension and split point
	if (size == 0) {
		return nullptr;
	}
	else {
		UL split;
		coordinate dom_elt;
		ChooseSplit(exm_set, size, split, dom_elt);//������ʼ�зַ������ʼ�ڵ�
		coordinate exm_set_right[KDtreeSize];//�洢λ�����ӿռ�ĵ�
		coordinate exm_set_left[KDtreeSize]; //�洢λ�����ӿռ�ĵ�
		UL size_left, size_right;
		size_left = size_right = 0;

		if (split == 0)
		{
			//��ʼ�зַ���Ϊx����
			for (UL i = 0; i < size; ++i)
			{
				//С�ڵ��ڽڵ�dom_elt.x��������ռ�
				if (!equal(exm_set[i], dom_elt) && exm_set[i].x <= dom_elt.x)
				{
					exm_set_left[size_left].x = exm_set[i].x;
					exm_set_left[size_left].y = exm_set[i].y;
					size_left++;
				}//���ڽڵ�dom_elt.x�������ҿռ�
				else if (!equal(exm_set[i], dom_elt) && exm_set[i].x > dom_elt.x)
				{
					exm_set_right[size_right].x = exm_set[i].x;
					exm_set_right[size_right].y = exm_set[i].y;
					size_right++;
				}
			}
		}
		else {
			//��ʼ�зַ���Ϊy����
			for (UL i = 0; i < size; ++i)
			{
				//С�ڵ��ڽڵ�dom_elt.y��������ռ�
				if (!equal(exm_set[i], dom_elt) && exm_set[i].y <= dom_elt.y)
				{
					exm_set_left[size_left].x = exm_set[i].x;
					exm_set_left[size_left].y = exm_set[i].y;
					size_left++;
				}//���ڽڵ�dom_elt.y��������ռ�
				else if (!equal(exm_set[i], dom_elt) && exm_set[i].y > dom_elt.y)
				{
					exm_set_right[size_right].x = exm_set[i].x;
					exm_set_right[size_right].y = exm_set[i].y;
					size_right++;
				}
			}
		}
		T = new TreeNode;
		T->dom_elt.x = dom_elt.x;
		T->dom_elt.y = dom_elt.y;
		T->split = split;
		T->left = build_kdtree(exm_set_left, size_left, T->left);    //�ݹ�
		T->right = build_kdtree(exm_set_right, size_right, T->right);//�ݹ�
		return T;
	}
}

//������������
double Distance(coordinate a, coordinate b) {
	double tmp = (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	return sqrt(tmp);
}

//�������ڽ���
void searchNearest(TreeNode * Kd, coordinate target, coordinate &nearestpoint, double & distance) {

	//1. ���Kd�ǿյģ�����distΪ����󷵻�
	//2. ��������ֱ��Ҷ�ӽ��
	stack<TreeNode*> search_path;
	TreeNode* pSearch = Kd;
	coordinate nearest;
	double dist;

	while (pSearch != nullptr)
	{
		//pSearch���뵽search_path��;
		search_path.push(pSearch);

		if (pSearch->split == 0)
		{
			if (target.x <= pSearch->dom_elt.x) /* ���С�ھͽ��������� */
			{
				pSearch = pSearch->left;
			}
			else
			{
				pSearch = pSearch->right;
			}
		}
		else {
			if (target.y <= pSearch->dom_elt.y) /* ���С�ھͽ��������� */
			{
				pSearch = pSearch->left;
			}
			else
			{
				pSearch = pSearch->right;
			}
		}
	}
	//ȡ��search_path���һ������nearest
	nearest.x = search_path.top()->dom_elt.x;
	nearest.y = search_path.top()->dom_elt.y;
	search_path.pop();

	dist = Distance(nearest, target);
	//3. ��������·��
	TreeNode* pBack;

	while (search_path.empty())
	{
		//ȡ��search_path���һ����㸳��pBack
		pBack = search_path.top();
		search_path.pop();

		if (pBack->left == nullptr && pBack->right == nullptr) /* ���pBackΪҶ�ӽ�� */
		{
			if (Distance(nearest, target) > Distance(pBack->dom_elt, target))
			{
				nearest = pBack->dom_elt;
				dist = Distance(pBack->dom_elt, target);
			}
		}
		else
		{
			UL s = pBack->split;
			if (s == 0)
			{
				if (fabs(pBack->dom_elt.x - target.x) < dist) /* �����targetΪ���ĵ�Բ������򣩣��뾶Ϊdist��Բ��ָƽ���ཻ�� ��ô��Ҫ������һ�ߵ��ӿռ�ȥ���� */
				{
					if (Distance(nearest, target) > Distance(pBack->dom_elt, target))
					{
						nearest = pBack->dom_elt;
						dist = Distance(pBack->dom_elt, target);
					}
					if (target.x <= pBack->dom_elt.x) /* ���targetλ��pBack�����ӿռ䣬��ô��Ҫ�������ӿռ�ȥ���� */
						pSearch = pBack->right;
					else
						pSearch = pBack->left; /* ���targetλ��pBack�����ӿռ䣬��ô��Ҫ�������ӿռ�ȥ���� */
					if (pSearch != nullptr)
						//pSearch���뵽search_path��
						search_path.push(pSearch);
				}
			}
			else {
				if (fabs(pBack->dom_elt.y - target.y) < dist) /* �����targetΪ���ĵ�Բ������򣩣��뾶Ϊdist��Բ��ָƽ���ཻ�� ��ô��Ҫ������һ�ߵ��ӿռ�ȥ���� */
				{
					if (Distance(nearest, target) > Distance(pBack->dom_elt, target))
					{
						nearest = pBack->dom_elt;
						dist = Distance(pBack->dom_elt, target);
					}
					if (target.y <= pBack->dom_elt.y) /* ���targetλ��pBack�����ӿռ䣬��ô��Ҫ�������ӿռ�ȥ���� */
						pSearch = pBack->right;
					else
						pSearch = pBack->left; /* ���targetλ��pBack�����ӿռ䣬��ô��Ҫ�������ӿռ�ȥ���� */
					if (pSearch != nullptr)
						// pSearch���뵽search_path��
						search_path.push(pSearch);
				}
			}

		}
	}

	nearestpoint.x = nearest.x;
	nearestpoint.y = nearest.y;
	distance = dist;
}

void test_kdtree() {
	coordinate exm_set[6];
	exm_set[0].x = 2;    exm_set[0].y = 3;
	exm_set[1].x = 5;    exm_set[1].y = 4;
	exm_set[2].x = 9;    exm_set[2].y = 6;
	exm_set[3].x = 4;    exm_set[3].y = 7;
	exm_set[4].x = 8;    exm_set[4].y = 1;
	exm_set[5].x = 7;    exm_set[5].y = 2;

	struct TreeNode * root = nullptr;
	root = build_kdtree(exm_set, 6, root);
	coordinate nearestpoint;
	double distance;
	coordinate target;
	target.x = 5.1;
	target.y = 4.1;

	searchNearest(root, target, nearestpoint, distance);
	cout << "The nearest distance is " << distance << ",and the nearest point is " << nearestpoint.x << "," << nearestpoint.y << endl;
}


