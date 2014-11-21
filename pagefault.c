/*
 * By Sanhu Li
 *
 * CS 441/541: Page Fault Algorithm Simulator (Project 4)
 */
#include "pagefault.h"


int main(int argc, char **argv) {
    int num_frames = -1, i;
    char buf[MAX_LINE], fn[MAX_LINE];
    FILE *in;

    long num_pages, j;
    int val;
    char rw;
    page *pages;

    strcpy(buf, "");

    if (argc > 1) {
        for (i = 1; i < argc; ++i) {
            if (strcmp("-f", argv[i])) {
                strcpy(fn, argv[i]);
                break;
            } else {
                i++;
            }
        }

        if (argc >= 4) {
            for (i = 1; i < argc; ++i) {
                if (!strcmp("-f", argv[i]) && i + 1 < argc) {
                    num_frames = strtol(argv[i + 1], NULL, 10);
                    break;
                }
            }
        }
    } else {
        usage();
        return 0;
    }

    if (!strcmp("", fn)) {
        usage();
        return 0;
    }

    in = fopen(fn, "r");
    fgets(buf, MAX_LINE, in);
    num_pages = strtol(buf, NULL, 10);

    pages = (page *) malloc(sizeof(pages) * num_pages);

    if (-1 == num_frames) {
        printf("Num. Frames : All\n");
        printf("Ref. File   : %s\n", fn);
        printf("-------------------------------\n");
        printf("Reference String:\n");

        for (j = 0; j < num_pages; ++j) {
            fscanf(in, "%d %c\n", &val, &rw);
            pages[j] = build_page(val, rw);
            if (j == num_pages - 1) {
                printf(" %d:%c\n", val, rw);
            } else {
                printf(" %d:%c, ", val, rw);
            }

            if (j % 10 == 9) {
                printf("\n");
            }
        }
        fclose(in);

        printf("-------------------------------\n");
        printf("####################################################\n");
        printf("#     Frames    Opt.    FIFO     LRU      SC     ESC\n");

        for (i = 1; i <= 7; ++i) {
            printf("# ");
            printf("%10d", i);
            printf("%8ld", alg_optimal(pages, num_pages, i));
            printf("%8ld", alg_fifo(pages, num_pages, i));
            printf("%8ld", alg_lru(pages, num_pages, i));
            printf("%8ld", alg_lru_sc(pages, num_pages, i));
            printf("%8ld\n", alg_lru_esc(pages, num_pages, i));
        }

        printf("####################################################\n");
    } else {
        printf("Num. Frames : %d\n", num_frames);
        printf("Ref. File   : %s\n", fn);
        printf("-------------------------------\n");
        printf("Reference String:\n");

        for (j = 0; j < num_pages; ++j) {
            fscanf(in, "%d %c\n", &val, &rw);
            pages[j] = build_page(val, rw);
            if (j == num_pages - 1) {
                printf(" %d:%c\n", val, rw);
            } else {
                printf(" %d:%c, ", val, rw);
            }

            if (j % 10 == 9) {
                printf("\n");
            }
        }
        fclose(in);

        printf("-------------------------------\n");
        printf("####################################################\n");
        printf("#     Frames    Opt.    FIFO     LRU      SC     ESC\n");

        printf("# ");
        printf("%10d", num_frames);
        printf("%8ld", alg_optimal(pages, num_pages, num_frames));
        printf("%8ld", alg_fifo(pages, num_pages, num_frames));
        printf("%8ld", alg_lru(pages, num_pages, num_frames));
        printf("%8ld", alg_lru_sc(pages, num_pages, num_frames));
        printf("%8ld\n", alg_lru_esc(pages, num_pages, num_frames));

        printf("####################################################\n");

//        debug_alg_optimal(pages, num_pages, num_frames);
//        debug_alg_fifo(pages, num_pages, num_frames);
//        debug_alg_lru(pages, num_pages, num_frames);
//        debug_alg_lru_sc(pages, num_pages, num_frames);
//        debug_alg_lru_esc(pages, num_pages, num_frames);
    }

    return 0;
}

long alg_optimal(page *pages, long n, int num_frames) {
    page frames[num_frames], cur_page;
    int i;
    long cur = 0, pf_count = 0, pg_idx;

    for (i = 0; i < num_frames; ++i) {
        frames[i] = build_page(-1, 'u');
    }

    while (cur < n) {
        cur_page = pages[cur];
        if (-1 == contains(frames, num_frames, cur_page.val)) {
            pf_count++;
            pg_idx = find_optimal_victim(pages + cur, n - cur, frames, num_frames);
            frames[pg_idx] = build_page(cur_page.val, cur_page.rw);
        }
        cur++;
    }
    return pf_count;
}

long find_optimal_victim(page *pages, long n, page *frames, int num_frames) {
    long distances[num_frames];
    long i, max = -1, victim = 0;
    int val;

    for (i = 0; i < num_frames; ++i) {
        if (frames[i].val == -1) {
            return i;
        }
    }

    for (i = 0; i < num_frames; ++i) {
        val = frames[i].val;
        distances[i] = contains(pages, n, val);
    }

    for (i = 0; i < num_frames; ++i) {
        if (distances[i] == -1) {
            return i;
        } else if (max < distances[i]) {
            max = distances[i];
            victim = i;
        }
    }
    return victim;
}

long alg_fifo(page *pages, long n, int num_frames) {
    page frames[num_frames], cur_page;
    int i, pg_idx = -1;
    long cur = 0, pf_count = 0;

    for (i = 0; i < num_frames; ++i) {
        frames[i] = build_page(-1, 'u');
    }

    while (cur < n) {
        cur_page = pages[cur];
        if (-1 == contains(frames, num_frames, cur_page.val)) {
            pf_count++;
            pg_idx = next(pg_idx, num_frames);
            frames[pg_idx] = build_page(cur_page.val, cur_page.rw);
        }
        cur++;
    }
    return pf_count;
}

long alg_lru(page *pages, long n, int num_frames) {
    page frames[num_frames], cur_page;
    int i, timestamps[num_frames];
    long cur = 0, pf_count = 0, pg_idx = -1, cresult;

    for (i = 0; i < num_frames; ++i) {
        frames[i] = build_page(-1, 'u');
        timestamps[i] = -1;
    }

    while (cur < n) {
        cur_page = pages[cur];
        cresult = contains(frames, num_frames, cur_page.val);
        if (-1 == cresult) {
            pf_count++;
            pg_idx = find_lru_victim(frames, timestamps, num_frames, n);
            if (pg_idx == -1) {
                printf("Error!!!");
                exit(-1);
            }
            frames[pg_idx] = build_page(cur_page.val, cur_page.rw);
            timestamps[pg_idx] = cur;
        } else {
            timestamps[cresult] = cur;
        }
        cur++;
    }
    return pf_count;
}

long find_lru_victim(page *frames, int *timestamps, int num_frames, long n) {
    long i;
    long min = n, idx;

    for (i = 0; i < num_frames; ++i) {
        if (frames[i].val == -1) {
            return i;
        }
    }

    for (i = 0; i < num_frames; ++i) {
        if (timestamps[i] < min) {
            idx = i;
            min = timestamps[i];
        }
    }

    return idx;
}

long alg_lru_sc(page *pages, long n, int num_frames) {
    page frames[num_frames], cur_page;
    int i, ref_bits[num_frames];
    long cur = 0, pf_count = 0, pg_idx = -1, cresult;

    for (i = 0; i < num_frames; ++i) {
        frames[i] = build_page(-1, 'u');
        ref_bits[i] = 0;
    }

    while (cur < n) {
        cur_page = pages[cur];
        cresult = contains(frames, num_frames, cur_page.val);
        if (-1 == cresult) {
            pf_count++;
            pg_idx = next(pg_idx, num_frames);
            while (ref_bits[pg_idx] == 1) {
                ref_bits[pg_idx] = 0;
                pg_idx = next(pg_idx, num_frames);
            }
            frames[pg_idx] = build_page(cur_page.val, cur_page.rw);
            ref_bits[pg_idx] = 1;
        } else {
            ref_bits[cresult] = 1;
        }
        cur++;
    }
    return pf_count;
}

long find_esc_victim(int *ref_bits, int *write_bits, int num_frames, int index) {
    int i, idx = next(index, num_frames);
    for (i = 0; i < num_frames; ++i) {
        if (ref_bits[idx] == 0 && write_bits[idx] == 0) {
            return idx;
        }
        idx = next(idx, num_frames);
    }

    for (i = 0; i < num_frames; ++i) {
        if (ref_bits[idx] == 0 && write_bits[idx] == 1) {
            return idx;
        }
        ref_bits[idx] = 0;
        idx = next(idx, num_frames);
    }

    for (i = 0; i < num_frames; ++i) {
        if (ref_bits[idx] == 0 && write_bits[idx] == 0) {
            return idx;
        }
        idx = next(idx, num_frames);
    }

    for (i = 0; i < num_frames; ++i) {
        if (ref_bits[idx] == 0 && write_bits[idx] == 1) {
            return idx;
        }
        idx = next(idx, num_frames);
    }
    return idx;
}

long alg_lru_esc(page *pages, long n, int num_frames) {
    page frames[num_frames], cur_page;
    int i, ref_bits[num_frames], write_bits[num_frames];
    long cur = 0, pf_count = 0, pg_idx = -1, cresult;

    for (i = 0; i < num_frames; ++i) {
        frames[i] = build_page(-1, 'u');
        ref_bits[i] = 0;
        write_bits[i] = 0;
    }

    while (cur < n) {
        cur_page = pages[cur];
        cresult = contains(frames, num_frames, cur_page.val);
        if (-1 == cresult) {
            pf_count++;
            pg_idx = find_esc_victim(ref_bits, write_bits, num_frames, pg_idx);
            frames[pg_idx] = build_page(cur_page.val, cur_page.rw);
            if ('r' == cur_page.rw || 'R' == cur_page.rw) {
                ref_bits[pg_idx] = 1;
                write_bits[pg_idx] = 0;
            } else if ('w' == cur_page.rw || 'W' == cur_page.rw) {
                ref_bits[pg_idx] = 1;
                write_bits[pg_idx] = 1;
            }
            fflush(NULL);
        } else {
            if ('r' == cur_page.rw || 'R' == cur_page.rw) {
                ref_bits[cresult] = 1;
            } else if ('w' == cur_page.rw || 'W' == cur_page.rw) {
                ref_bits[cresult] = 1;
                write_bits[cresult] = 1;
            }
        }
        cur++;
    }
    return pf_count;
}

int next(int cur, int max) {
    return ++cur == max ? 0 : cur;
}

long contains(page *frames, long n, int p_val) {
    long i;
    for (i = 0; i < n; ++i) {
        if (frames[i].val == p_val) {
            return i;
        }
    }
    return -1;
}

page build_page(int val, char rw) {
    page result;
    result.val = val;
    result.rw = rw;
    return result;
}

void usage() {
    printf("./pagefault filename [-f num_of_frames]\n");
}

void print_frames(page *frames, int num_frames, bool ignore_ref, bool ignore_write, int *ref_bits, int *write_bits, long idx) {
    int i;
    static int lineno = 0;
    char * template = " %4d(%d,%d)";
    if (ignore_ref) {
        template = " %4d";
    } else if (ignore_write) {
        template = " %4d(%d)";
    }
    printf("\n%3d", ++lineno);
    for (i = 0; i < num_frames; ++i) {
        if (ignore_ref) {
            printf(template, frames[i].val);
        } else if (ignore_write) {
            printf(template, frames[i].val, ref_bits[i]);
        } else {
            printf(template, frames[i].val, ref_bits[i], write_bits[i]);
        }
        if (idx == i) {
            printf("^");
        }
    }
    printf("\n");
}
