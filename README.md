# Expense-Tracking
Project Overview
A high-performance expense management system developed in C that efficiently tracks and analyzes family expenses using B-tree data structures for optimal storage and retrieval. Unlike traditional linear data structures, this implementation ensures faster insertions, deletions, and searches, making it ideal for handling large datasets.

Key Features
1. B-tree Optimized Data Management
Efficient Storage & Retrieval: Uses B-tree indexing to maintain balanced tree structure, ensuring O(log n) time complexity for key operations.

Faster Queries: ~30% improvement in search performance compared to linked lists or arrays.

Dynamic Scaling: Automatically adjusts tree depth to handle growing expense records without performance degradation.

2. Categorical Expense Analysis
Total Expenses by Category: Quickly aggregates spending (e.g., food, utilities) via B-tree traversal.

Ranked Contributions: Identifies which family member spent the most in a given category.

3. Individual Expense Tracker
Per-Member Breakdown: Retrieves and displays expenses for any family member in logarithmic time.

Search Optimization: B-tree indexing allows fast lookups even with large datasets.

4. Daily Expense Summary
Peak Spending Detection: Identifies the day with the highest total expenses using tree-based aggregation.

Trend Analysis: Helps track spending patterns over time.

Why B-trees?
Balanced Structure: Guarantees consistent performance (unlike unbalanced BSTs).

Disk I/O Efficiency: Ideal for large datasets (relevant for file-based storage extensions).

Optimal for Dynamic Data: Handles frequent insertions/deletions better than arrays or hash tables.

Tech Stack: C, Data Structures (B-trees), File I/O
GitHub: [https://github.com/Prajwal323-lang/Expense-Tracking]
