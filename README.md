# Filters_RTS
Real Time System Project: Simulate Filters with pthread library.

Author: Daniele Caradonna.

### Note to compile and run
- **To compile**: `/project_folder/make`
- **To run**: `sudo ./filters`

### Periodic Task
This task stamp Hello World and checks deadline misses.

```
void *helloWorldTask(void* arg)
{
    int idx;

    idx = get_task_index(arg);
    set_activation(idx);

    while(!end_flag)
    {
        printf("Hello World! \n");
        
        if(deadline_miss(idx))
            printf("******Deadline Miss!******** \n");

        wait_for_activation(idx);
    }

    return NULL;
}
```

In `main()`:

```
int main(void)
{
    int task_report; // 0: Ok | 1: Task Issues

    task_report = task_create(helloWorldTask, IDX, PER, PER, PRIO);
    printf("Hello World Task! Report: %d \n", task_report);
    wait_for_task(IDX);

    return 0;
}
```

https://rosettacode.org/wiki/Fast_Fourier_transform#C

https://www.codeproject.com/Articles/9388/How-to-implement-the-FFT-algorithm