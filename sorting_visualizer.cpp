#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
#include <sstream>

class SortingVisualizer {
private:
    sf::RenderWindow& window;
    std::vector<int> arr;
    std::vector<sf::RectangleShape> bars;
    float barWidth;
    int delayMs;
    bool isSorting;
    bool isPaused;
    std::string currentAlgorithm;
    std::string algorithmDetails;
    sf::Font font;
    sf::Text detailsText;
    sf::Text instructionsText;
    sf::Text statsText;
    std::vector<sf::Text> algorithmButtons;
    unsigned long long comparisons;
    unsigned long long swaps;
    sf::RectangleShape controlPanel;
    sf::RectangleShape statsPanel;
    size_t activeAlgorithmIndex;

    void setBarColor(size_t index, sf::Color color) {
        if (index < bars.size()) {
            bars[index].setFillColor(color);
        }
    }

    void updateBar(size_t index, int value) {
        if (index < bars.size()) {
            arr[index] = value;
            bars[index].setSize(sf::Vector2f(barWidth - 1, value));
            bars[index].setPosition(index * barWidth, 600 - value);
        }
    }

    void updateStats() {
        std::stringstream ss;
        ss << "Comparisons: " << comparisons << "\nSwaps: " << swaps << "\nSpeed: " << (1000.0 / delayMs) << " ops/sec";
        statsText.setString(ss.str());
    }

    std::string wrapText(const std::string& text, float maxWidth) {
        std::string wrapped;
        std::string currentLine;
        size_t lastSpace = 0;
        float currentWidth = 0;
        sf::Text tempText("", font, 16);
        for (size_t i = 0; i < text.length(); ++i) {
            currentLine += text[i];
            tempText.setString(currentLine);
            currentWidth = tempText.getGlobalBounds().width;
            if (text[i] == ' ' || text[i] == '\n') {
                lastSpace = i;
            }
            if (currentWidth > maxWidth || text[i] == '\n') {
                if (lastSpace > 0 && text[i] != '\n') {
                    wrapped += currentLine.substr(0, lastSpace) + "\n";
                    currentLine = currentLine.substr(lastSpace + 1);
                    i = lastSpace;
                } else {
                    wrapped += currentLine + "\n";
                    currentLine = "";
                }
                lastSpace = 0;
                tempText.setString(currentLine);
                currentWidth = tempText.getGlobalBounds().width;
            }
        }
        wrapped += currentLine;
        return wrapped;
    }

public:
    SortingVisualizer(sf::RenderWindow& win, int size = 100, int delay = 5)
        : window(win), delayMs(delay), isSorting(false), isPaused(false), currentAlgorithm("None"), comparisons(0), swaps(0), activeAlgorithmIndex(-1) {
        arr.resize(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10, 400);
        for (int i = 0; i < size; ++i) {
            arr[i] = dis(gen);
        }

        barWidth = 800.0f / size;
        bars.resize(size);
        for (size_t i = 0; i < size; ++i) {
            bars[i].setSize(sf::Vector2f(barWidth - 1, arr[i]));
            bars[i].setPosition(i * barWidth, 600 - arr[i]);
            bars[i].setFillColor(sf::Color(75, 156, 234)); // Blue
        }

        if (!font.loadFromFile("/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf")) {
            detailsText.setString("Font failed to load. Algorithm: None");
        } else {
            // Instructions
            instructionsText.setFont(font);
            instructionsText.setCharacterSize(14);
            instructionsText.setFillColor(sf::Color::White);
            instructionsText.setStyle(sf::Text::Bold);
            instructionsText.setPosition(10, 5);
            instructionsText.setString("Keys 1-0: Select algorithm, R: Reset, P: Pause/Resume, +/-: Speed");

            // Stats
            statsText.setFont(font);
            statsText.setCharacterSize(14);
            statsText.setFillColor(sf::Color::White);
            statsText.setPosition(600, 100);
            updateStats();

            // Stats panel
            statsPanel.setSize(sf::Vector2f(180, 80));
            statsPanel.setPosition(590, 90);
            statsPanel.setFillColor(sf::Color(50, 50, 50, 200));

            // Details
            detailsText.setFont(font);
            detailsText.setCharacterSize(16);
            detailsText.setFillColor(sf::Color::White);
            detailsText.setStyle(sf::Text::Bold);
            detailsText.setPosition(10, 100);
            updateDetails("Select an algorithm by clicking or using keys 1-0.");

            // Control panel
            controlPanel.setSize(sf::Vector2f(780, 90));
            controlPanel.setPosition(10, 10);
            controlPanel.setFillColor(sf::Color(50, 50, 50, 200));

            // Algorithm buttons
            std::vector<std::string> algoNames = {
                "1: Bubble", "2: Selection", "3: Insertion", "4: Quick", "5: Merge",
                "6: Heap", "7: Tim", "8: Counting", "9: Radix", "0: Bucket"
            };
            float x = 20, y = 35;
            for (const auto& name : algoNames) {
                sf::Text button;
                button.setFont(font);
                button.setString(name);
                button.setCharacterSize(14);
                button.setFillColor(sf::Color::White);
                button.setPosition(x, y);
                algorithmButtons.push_back(button);
                x += 80;
                if (x > 700) {
                    x = 20;
                    y += 20; // Second row at y=55
                }
            }
        }
    }

    void draw() {
        window.clear(sf::Color(46, 46, 46)); // Dark gray
        window.draw(controlPanel);
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (size_t i = 0; i < algorithmButtons.size(); ++i) {
            bool isHovered = algorithmButtons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            algorithmButtons[i].setFillColor(i == activeAlgorithmIndex ? sf::Color(85, 255, 85) : (isHovered ? sf::Color(255, 255, 85) : sf::Color::White));
            window.draw(algorithmButtons[i]);
        }
        window.draw(instructionsText);
        window.draw(statsPanel);
        window.draw(statsText);
        if (isPaused) {
            detailsText.setFillColor(sf::Color(255, 85, 85));
        } else {
            detailsText.setFillColor(sf::Color::White);
        }
        window.draw(detailsText);
        for (const auto& bar : bars) {
            window.draw(bar);
        }
        window.display();
        if (isSorting && !isPaused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }

    void updateDetails(const std::string& details) {
        algorithmDetails = wrapText("Algorithm: " + currentAlgorithm + (isPaused ? "\nPaused" : "") + "\n" + details, 780);
        detailsText.setString(algorithmDetails);
    }

    void resetColors() {
        for (auto& bar : bars) {
            bar.setFillColor(isSorting ? sf::Color(75, 156, 234) : sf::Color(85, 255, 85)); // Green when sorted
        }
    }

    void resetArray() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10, 400);
        for (size_t i = 0; i < arr.size(); ++i) {
            updateBar(i, dis(gen));
            bars[i].setFillColor(sf::Color(75, 156, 234));
        }
        currentAlgorithm = "None";
        activeAlgorithmIndex = -1;
        comparisons = 0;
        swaps = 0;
        updateStats();
        updateDetails("Select an algorithm by clicking or using keys 1-0.");
        draw();
    }

    void handleMouseClick(sf::Vector2i mousePos) {
        if (isSorting && !isPaused) return;
        for (size_t i = 0; i < algorithmButtons.size(); ++i) {
            if (algorithmButtons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                activeAlgorithmIndex = i;
                switch (i) {
                    case 0: bubbleSort(); break;
                    case 1: selectionSort(); break;
                    case 2: insertionSort(); break;
                    case 3: startQuickSort(); break;
                    case 4: startMergeSort(); break;
                    case 5: startHeapSort(); break;
                    case 6: timSort(); break;
                    case 7: countingSort(); break;
                    case 8: radixSort(); break;
                    case 9: bucketSort(); break;
                }
                break;
            }
        }
    }

    void bubbleSort() {
        isSorting = true;
        currentAlgorithm = "Bubble Sort";
        activeAlgorithmIndex = 0;
        updateDetails("Time: O(n^2)\nSpace: O(1)\nCompares adjacent elements, swaps if out of order.");
        comparisons = 0;
        swaps = 0;
        for (size_t i = 0; i < arr.size() - 1 && isSorting; ++i) {
            bool swapped = false;
            for (size_t j = 0; j < arr.size() - i - 1 && isSorting && !isPaused; ++j) {
                setBarColor(j, sf::Color(255, 85, 85));
                setBarColor(j + 1, sf::Color(255, 85, 85));
                draw();
                comparisons++;
                if (arr[j] > arr[j + 1]) {
                    std::swap(arr[j], arr[j + 1]);
                    updateBar(j, arr[j]);
                    updateBar(j + 1, arr[j + 1]);
                    swaps++;
                    swapped = true;
                }
                updateStats();
                setBarColor(j, sf::Color(75, 156, 234));
                setBarColor(j + 1, sf::Color(75, 156, 234));
            }
            if (!swapped) break;
        }
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void selectionSort() {
        isSorting = true;
        currentAlgorithm = "Selection Sort";
        activeAlgorithmIndex = 1;
        updateDetails("Time: O(n^2)\nSpace: O(1)\nFinds minimum in unsorted portion, places at start.");
        comparisons = 0;
        swaps = 0;
        for (size_t i = 0; i < arr.size() - 1 && isSorting; ++i) {
            size_t minIdx = i;
            for (size_t j = i + 1; j < arr.size() && isSorting && !isPaused; ++j) {
                setBarColor(j, sf::Color(255, 85, 85));
                setBarColor(minIdx, sf::Color(255, 255, 85));
                draw();
                comparisons++;
                if (arr[j] < arr[minIdx]) {
                    setBarColor(minIdx, sf::Color(75, 156, 234));
                    minIdx = j;
                }
                updateStats();
                setBarColor(j, sf::Color(75, 156, 234));
            }
            if (minIdx != i) {
                std::swap(arr[i], arr[minIdx]);
                updateBar(i, arr[i]);
                updateBar(minIdx, arr[minIdx]);
                swaps++;
            }
            setBarColor(i, sf::Color(75, 156, 234));
            setBarColor(minIdx, sf::Color(75, 156, 234));
        }
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void insertionSort() {
        isSorting = true;
        currentAlgorithm = "Insertion Sort";
        activeAlgorithmIndex = 2;
        updateDetails("Time: O(n^2)\nSpace: O(1)\nInserts each element into sorted portion.");
        comparisons = 0;
        swaps = 0;
        for (size_t i = 1; i < arr.size() && isSorting; ++i) {
            int key = arr[i];
            size_t j = i;
            setBarColor(j, sf::Color(255, 85, 85));
            draw();
            while (j > 0 && arr[j - 1] > key && isSorting && !isPaused) {
                comparisons++;
                arr[j] = arr[j - 1];
                updateBar(j, arr[j]);
                setBarColor(j - 1, sf::Color(255, 85, 85));
                draw();
                setBarColor(j - 1, sf::Color(75, 156, 234));
                --j;
                swaps++;
            }
            arr[j] = key;
            updateBar(j, arr[j]);
            setBarColor(j, sf::Color(75, 156, 234));
            updateStats();
        }
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    int partition(int low, int high) {
        if (!isSorting || isPaused) return low;
        int pivot = arr[high];
        setBarColor(high, sf::Color(255, 255, 85));
        int i = low - 1;
        for (int j = low; j < high && isSorting && !isPaused; ++j) {
            setBarColor(j, sf::Color(255, 85, 85));
            draw();
            comparisons++;
            if (arr[j] <= pivot) {
                ++i;
                std::swap(arr[i], arr[j]);
                updateBar(i, arr[i]);
                updateBar(j, arr[j]);
                swaps++;
            }
            setBarColor(j, sf::Color(75, 156, 234));
            updateStats();
        }
        std::swap(arr[i + 1], arr[high]);
        updateBar(i + 1, arr[i + 1]);
        updateBar(high, arr[high]);
        swaps++;
        setBarColor(high, sf::Color(75, 156, 234));
        setBarColor(i + 1, sf::Color(75, 156, 234));
        draw();
        return i + 1;
    }

    void quickSort(int low, int high) {
        if (!isSorting || low >= high || isPaused) return;
        int pi = partition(low, high);
        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }

    void startQuickSort() {
        isSorting = true;
        currentAlgorithm = "Quick Sort";
        activeAlgorithmIndex = 3;
        updateDetails("Time: O(n log n) avg, O(n^2) worst\nSpace: O(log n)\nPartitions around a pivot.");
        comparisons = 0;
        swaps = 0;
        quickSort(0, arr.size() - 1);
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void merge(int left, int mid, int right) {
        if (!isSorting || isPaused) return;
        std::vector<int> temp(right - left + 1);
        size_t i = left, j = mid + 1, k = 0;
        while (i <= mid && j <= right && isSorting && !isPaused) {
            setBarColor(i, sf::Color(255, 85, 85));
            setBarColor(j, sf::Color(255, 85, 85));
            draw();
            comparisons++;
            if (arr[i] <= arr[j]) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
            }
            setBarColor(i, sf::Color(75, 156, 234));
            setBarColor(j, sf::Color(75, 156, 234));
            updateStats();
        }
        while (i <= mid && isSorting && !isPaused) {
            temp[k++] = arr[i++];
            setBarColor(i, sf::Color(75, 156, 234));
        }
        while (j <= right && isSorting && !isPaused) {
            temp[k++] = arr[j++];
            setBarColor(j, sf::Color(75, 156, 234));
        }
        for (size_t idx = 0; idx < temp.size() && isSorting && !isPaused; ++idx) {
            updateBar(left + idx, temp[idx]);
            setBarColor(left + idx, sf::Color(85, 255, 85));
            draw();
            setBarColor(left + idx, sf::Color(75, 156, 234));
        }
    }

    void mergeSort(int left, int right) {
        if (!isSorting || left >= right || isPaused) return;
        int mid = left + (right - left) / 2;
        mergeSort(left, mid);
        mergeSort(mid + 1, right);
        merge(left, mid, right);
    }

    void startMergeSort() {
        isSorting = true;
        currentAlgorithm = "Merge Sort";
        activeAlgorithmIndex = 4;
        updateDetails("Time: O(n log n)\nSpace: O(n)\nDivides and merges sorted halves.");
        comparisons = 0;
        swaps = 0;
        mergeSort(0, arr.size() - 1);
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void heapify(int n, int i) {
        if (!isSorting || isPaused) return;
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < n && arr[left] > arr[largest]) {
            comparisons++;
            largest = left;
        }
        if (right < n && arr[right] > arr[largest]) {
            comparisons++;
            largest = right;
        }
        if (largest != i && isSorting && !isPaused) {
            std::swap(arr[i], arr[largest]);
            updateBar(i, arr[i]);
            updateBar(largest, arr[largest]);
            swaps++;
            setBarColor(i, sf::Color(255, 85, 85));
            setBarColor(largest, sf::Color(255, 85, 85));
            draw();
            setBarColor(i, sf::Color(75, 156, 234));
            setBarColor(largest, sf::Color(75, 156, 234));
            heapify(n, largest);
        }
        updateStats();
    }

    void startHeapSort() {
        isSorting = true;
        currentAlgorithm = "Heap Sort";
        activeAlgorithmIndex = 5;
        updateDetails("Time: O(n log n)\nSpace: O(1)\nUses a max heap to sort elements.");
        comparisons = 0;
        swaps = 0;
        int n = arr.size();
        for (int i = n / 2 - 1; i >= 0 && isSorting && !isPaused; --i) {
            heapify(n, i);
        }
        for (int i = n - 1; i > 0 && isSorting && !isPaused; --i) {
            std::swap(arr[0], arr[i]);
            updateBar(0, arr[0]);
            updateBar(i, arr[i]);
            swaps++;
            draw();
            heapify(i, 0);
        }
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void timSort() {
        isSorting = true;
        currentAlgorithm = "Tim Sort";
        activeAlgorithmIndex = 6;
        updateDetails("Time: O(n log n)\nSpace: O(n)\nHybrid of merge and insertion sort.");
        comparisons = 0;
        swaps = 0;
        const int RUN = 32;
        for (size_t i = 0; i < arr.size() && isSorting && !isPaused; i += RUN) {
            for (size_t j = i + 1; j < std::min(i + RUN, arr.size()) && isSorting && !isPaused; ++j) {
                int key = arr[j];
                size_t k = j;
                setBarColor(k, sf::Color(255, 85, 85));
                draw();
                while (k > i && arr[k - 1] > key && isSorting && !isPaused) {
                    comparisons++;
                    arr[k] = arr[k - 1];
                    updateBar(k, arr[k]);
                    setBarColor(k - 1, sf::Color(255, 85, 85));
                    draw();
                    setBarColor(k - 1, sf::Color(75, 156, 234));
                    --k;
                    swaps++;
                }
                arr[k] = key;
                updateBar(k, arr[k]);
                setBarColor(k, sf::Color(75, 156, 234));
                updateStats();
            }
        }
        for (size_t size = RUN; size < arr.size() && isSorting && !isPaused; size *= 2) {
            for (size_t left = 0; left < arr.size() && isSorting && !isPaused; left += 2 * size) {
                size_t mid = left + size - 1;
                size_t right = std::min(left + 2 * size - 1, arr.size() - 1);
                if (mid < right) {
                    merge(left, mid, right);
                }
            }
        }
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void countingSort() {
        isSorting = true;
        currentAlgorithm = "Counting Sort";
        activeAlgorithmIndex = 7;
        updateDetails("Time: O(n + k)\nSpace: O(k)\nCounts occurrences to sort non-negative integers.");
        comparisons = 0;
        swaps = 0;
        int maxVal = *std::max_element(arr.begin(), arr.end());
        std::vector<int> count(maxVal + 1, 0);
        for (size_t i = 0; i < arr.size() && isSorting && !isPaused; ++i) {
            ++count[arr[i]];
            setBarColor(i, sf::Color(255, 85, 85));
            draw();
            setBarColor(i, sf::Color(75, 156, 234));
        }
        size_t k = 0;
        for (int i = 0; i <= maxVal && isSorting && !isPaused; ++i) {
            while (count[i] > 0 && isSorting && !isPaused) {
                updateBar(k, i);
                setBarColor(k, sf::Color(85, 255, 85));
                draw();
                setBarColor(k, sf::Color(75, 156, 234));
                --count[i];
                ++k;
                swaps++;
            }
        }
        updateStats();
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void countingSortForRadix(int exp) {
        if (!isSorting || isPaused) return;
        std::vector<int> output(arr.size());
        std::vector<int> count(10, 0);
        for (size_t i = 0; i < arr.size() && isSorting && !isPaused; ++i) {
            ++count[(arr[i] / exp) % 10];
            setBarColor(i, sf::Color(255, 85, 85));
            draw();
            setBarColor(i, sf::Color(75, 156, 234));
        }
        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }
        for (int i = arr.size() - 1; i >= 0 && isSorting && !isPaused; --i) {
            output[count[(arr[i] / exp) % 10] - 1] = arr[i];
            --count[(arr[i] / exp) % 10];
        }
        for (size_t i = 0; i < arr.size() && isSorting && !isPaused; ++i) {
            updateBar(i, output[i]);
            setBarColor(i, sf::Color(85, 255, 85));
            draw();
            setBarColor(i, sf::Color(75, 156, 234));
            swaps++;
        }
        updateStats();
    }

    void radixSort() {
        isSorting = true;
        currentAlgorithm = "Radix Sort";
        activeAlgorithmIndex = 8;
        updateDetails("Time: O(nk)\nSpace: O(n + k)\nSorts by processing digits.");
        comparisons = 0;
        swaps = 0;
        int maxVal = *std::max_element(arr.begin(), arr.end());
        if (maxVal == 0) {
            isSorting = false;
            isPaused = false;
            resetColors();
            draw();
            return;
        }
        for (int exp = 1; maxVal / exp > 0 && isSorting && !isPaused; exp *= 10) {
            countingSortForRadix(exp);
        }
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void bucketSort() {
        isSorting = true;
        currentAlgorithm = "Bucket Sort";
        activeAlgorithmIndex = 9;
        updateDetails("Time: O(n + k)\nSpace: O(n + k)\nDistributes into buckets and sorts.");
        comparisons = 0;
        swaps = 0;
        int maxVal = *std::max_element(arr.begin(), arr.end());
        if (maxVal == 0) {
            isSorting = false;
            isPaused = false;
            resetColors();
            draw();
            return;
        }
        int bucketCount = arr.size() / 10 + 1;
        std::vector<std::vector<int>> buckets(bucketCount);
        for (size_t i = 0; i < arr.size() && isSorting && !isPaused; ++i) {
            int bucketIdx = arr[i] * bucketCount / (maxVal + 1);
            buckets[bucketIdx].push_back(arr[i]);
            setBarColor(i, sf::Color(255, 85, 85));
            draw();
            setBarColor(i, sf::Color(75, 156, 234));
        }
        size_t k = 0;
        for (auto& bucket : buckets) {
            std::sort(bucket.begin(), bucket.end());
            for (int val : bucket) {
                if (!isSorting || isPaused) break;
                updateBar(k, val);
                setBarColor(k, sf::Color(85, 255, 85));
                draw();
                setBarColor(k, sf::Color(75, 156, 234));
                ++k;
                swaps++;
            }
        }
        updateStats();
        isSorting = false;
        isPaused = false;
        resetColors();
        draw();
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::P) {
            if (isSorting) {
                isPaused = !isPaused;
                updateDetails(algorithmDetails);
                draw();
            }
            return;
        }
        if (key == sf::Keyboard::Add && delayMs > 1) {
            delayMs -= 1;
            updateStats();
            draw();
            return;
        }
        if (key == sf::Keyboard::Subtract && delayMs < 50) {
            delayMs += 1;
            updateStats();
            draw();
            return;
        }
        if (isSorting) {
            isSorting = false;
            isPaused = false;
            activeAlgorithmIndex = -1;
            resetColors();
            updateDetails("Sorting stopped. Select an algorithm.");
            draw();
            return;
        }
        switch (key) {
            case sf::Keyboard::Num1: bubbleSort(); break;
            case sf::Keyboard::Num2: selectionSort(); break;
            case sf::Keyboard::Num3: insertionSort(); break;
            case sf::Keyboard::Num4: startQuickSort(); break;
            case sf::Keyboard::Num5: startMergeSort(); break;
            case sf::Keyboard::Num6: startHeapSort(); break;
            case sf::Keyboard::Num7: timSort(); break;
            case sf::Keyboard::Num8: countingSort(); break;
            case sf::Keyboard::Num9: radixSort(); break;
            case sf::Keyboard::Num0: bucketSort(); break;
            case sf::Keyboard::R: resetArray(); break;
            default: break;
        }
    }

    bool isSortingNow() const { return isSorting; }
    bool isPausedNow() const { return isPaused; }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Sorting Visualizer");
    window.setFramerateLimit(60);
    SortingVisualizer visualizer(window, 100, 5);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                visualizer.handleKeyPress(event.key.code);
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                visualizer.handleMouseClick(sf::Mouse::getPosition(window));
            }
        }
        if (!visualizer.isSortingNow() || (visualizer.isSortingNow() && !visualizer.isPausedNow())) {
            visualizer.draw();
        }
    }
    return 0;
}