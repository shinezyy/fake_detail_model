#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "directed_tbs.h"


int d2q_fifo;
int q2d_fifo;

float cpi_list[] = {
    0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 4.0, 0.5, 1.0, 1.0, 0.5, 2.0, 0.5,
    1.0, 1.0, 4.0, 0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 4.0, 0.5, 1.0, 1.0,
    0.5, 2.0, 0.5, 1.0, 1.0, 4.0, 0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 4.0,
    0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 4.0, 0.5, 1.0, 1.0, 0.5, 2.0, 0.5,
    1.0, 1.0, 4.0, 0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 4.0, 0.5, 1.0, 1.0,
    0.5, 2.0, 0.5, 1.0, 1.0, 4.0, 0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 4.0,
    0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 4.0};

int main(int argc, char *argv[])
{
    const char *detail_to_qemu_fifo_name = "./detail_to_qemu.fifo";
    const char *qemu_to_detail_fifo_name = "./qemu_to_detail.fifo";

    mkfifo(detail_to_qemu_fifo_name, 0666);
    mkfifo(qemu_to_detail_fifo_name, 0666);

    d2q_fifo = open(detail_to_qemu_fifo_name, O_WRONLY);
    q2d_fifo = open(qemu_to_detail_fifo_name, O_RDONLY);

    Detail2Qemu d2q_buf;
    Qemu2Detail q2d_buf;

    for (int i = 0; i < 100; i += 2) {
        read(q2d_fifo, &q2d_buf, sizeof(Qemu2Detail));
        printf("Received from QEMU: %d %d %ld\n", q2d_buf.cpt_ready,
                q2d_buf.cpt_id, q2d_buf.total_inst_count);
        sleep(5);

        d2q_buf.CPI[i % 2] = cpi_list[i];
        d2q_buf.CPI[(i + 1) % 2] = cpi_list[i + 1];
        printf("Sending to QEMU: %f %f\n", d2q_buf.CPI[i % 2],
                d2q_buf.CPI[(i + 1) % 2]);
        write(d2q_fifo, &d2q_buf, sizeof(Detail2Qemu));
    }

    return 0;
}