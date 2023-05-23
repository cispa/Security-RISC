#! /usr/bin/env python3

import statistics
import sys

def main():
    if len(sys.argv) != 2:
        print(f"USAGE: {sys.argv[0]} <flush-fault.csv|fault-ret.csv>")
        exit(0)
    fname = sys.argv[1]
    cached_measurements = dict()
    uncached_measurements = dict()
    with open(fname) as fd:
        for line in fd.readlines()[20:]:
            idx, cached, value = line.split(",")
            idx = int(idx)
            cached = bool(int(cached))
            value = int(value)
            if cached:
                if idx in cached_measurements:
                    cached_measurements[idx].append(value)
                else:
                    cached_measurements[idx] = [value]
            else:
                if idx in uncached_measurements:
                    uncached_measurements[idx].append(value)
                else:
                    uncached_measurements[idx] = [value]
    
    median_cached = statistics.median([statistics.median(i) for i in cached_measurements.values()]) 
    median_uncached = statistics.median([statistics.median(i) for i in uncached_measurements.values()]) 
    min_cached = min([statistics.median(i) for i in cached_measurements.values()])
    min_uncached = min([statistics.median(i) for i in uncached_measurements.values()])
    print("median cached:\t\t", median_cached)
    print("median uncached:\t", median_uncached)
    print("min cached:\t\t", min_cached)
    print("min uncached:\t\t", min_uncached)

    threshold = (median_cached + median_uncached) / 2
    print(f"Threshold: {threshold}")
    true_pos = 0
    false_pos = 0
    true_neg = 0
    false_neg = 0
    pos_examples = len(cached_measurements)
    neg_examples = len(uncached_measurements)
    for measurements in cached_measurements.values():
        m = statistics.median(measurements)
        if m <= threshold:
            true_pos += 1
        else:
            false_neg += 1
    
    for measurements in uncached_measurements.values():
        m = statistics.median(measurements)
        if m > threshold:
            true_neg += 1
        else:
            false_pos += 1
    
    
    print(f"True Positive: {true_pos}")
    print(f"False Positive: {false_pos}")
    print(f"True Negative: {true_neg}")
    print(f"False Negative: {false_neg}")

    recall = true_pos / (true_pos + false_neg)
    precision = true_pos / (true_pos + false_pos)
    accuracy = (true_pos + true_neg) / (pos_examples + neg_examples)
    f_score = 2 * ((precision * recall) / (precision + recall))
    print("=============================================")
    print(f"Accuracy: {accuracy}")
    print(f"Recall: {recall}")
    print(f"Precision: {precision}")
    print(f"F1: {f_score}")
    
        
    



if __name__ == "__main__":
    main()
