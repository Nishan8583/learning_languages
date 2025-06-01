# Threads
- Use when high computation (CPU bound) is required that is not relevant with each others, dont create too many threads cause OS needs to context switch.
- NOt useful for network (We are taking about OS threads here), because OS will have to poll for each threads, and that will waste a lot of CPU cycles.