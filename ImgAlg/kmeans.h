#include <vector>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <ctime>

struct RGB
{
    unsigned short r, g, b;

    // 计算两个颜色之间的距离
    static double distance(const RGB& a, const RGB& b) 
	{
        return std::sqrt((double)std::pow(a.r - b.r, 2) + (double)std::pow(a.g - b.g, 2) + (double)std::pow(a.b - b.b, 2));
    }
};

class KMeans
{
public:
    KMeans(int k, int maxIterations) : k(k), maxIterations(maxIterations)
	{
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
		m_bExit = false;
    }

	void SetRemainCall(std::function<void(unsigned int)> fun) { TIME_REMAIN = fun; }
	void SetExit() { m_bExit = true; }

    void run(const std::vector<RGB>& data)
	{
        // 随机初始化质心
        for (int i = 0; i < k; ++i) 
		{
			if (m_bExit) return;

            int idx = std::rand() % data.size();
            centroids.push_back(data[idx]);
        }

		unsigned int pertime = 0;
		std::time_t start,end;
        for (int iter = 0; iter < maxIterations; ++iter) 
		{
			if (m_bExit) return;

			if (pertime == 0)
			{
				start = std::time(nullptr);
			}
            // 将每个点分配给最近的质心
            std::vector<std::vector<RGB>> clusters(k);
            for (const auto& point : data) 
			{
				if (m_bExit) return;
                int nearest = findNearestCentroid(point);
                clusters[nearest].push_back(point);
            }

            // 重新计算质心
            for (int i = 0; i < k; ++i) 
			{
				if (m_bExit) return;
                centroids[i] = calculateCentroid(clusters[i]);
            }

			if (pertime == 0)
			{
				end = std::time(nullptr);
				pertime = end - start;
			}
			else
			{
				unsigned int remain = pertime * (maxIterations - iter);
				TIME_REMAIN(remain);
			}
        }
    }

    std::vector<RGB> getClusterCenters() const
	{
        return centroids;
    }

private:
    int k;
    int maxIterations;
	bool m_bExit;
    std::vector<RGB> centroids;
	std::function<void(unsigned int)> TIME_REMAIN;

    int findNearestCentroid(const RGB& point) 
	{
		if (m_bExit) return 0;
        double minDistance = std::numeric_limits<double>::max();
        int nearest = 0;

        for (int i = 0; i < k; ++i) 
		{
			if (m_bExit) return 0;
            double dist = RGB::distance(point, centroids[i]);
            if (dist < minDistance) 
			{
                minDistance = dist;
                nearest = i;
            }
        }

        return nearest;
    }

    RGB calculateCentroid(const std::vector<RGB>& cluster)
	{
        double sumR = 0, sumG = 0, sumB = 0;
        for (const auto& point : cluster) 
		{
			if (m_bExit) return{0,0,0};
            sumR += point.r;
            sumG += point.g;
            sumB += point.b;
        }

        double size = cluster.size();
        return {(unsigned short)(sumR / size), (unsigned short)(sumG / size), (unsigned short)(sumB / size)};
    }
};
