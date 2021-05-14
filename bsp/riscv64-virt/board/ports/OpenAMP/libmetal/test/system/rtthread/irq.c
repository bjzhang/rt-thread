
#include <errno.h>
#include <metal/irq.h>	//metal_irq_register
#include <metal/irq_controller.h>
#include <metal/sys.h>
#include <metal/log.h>

static int irq_handler(int irq, void *priv)
{
    (void)irq;
    (void)priv;

    return 0;
}

int irq(void)
{
	int rc = 0;
	char *err_msg = "";

	rc = metal_irq_register(0, irq_handler, NULL);
	if (rc) {
		err_msg = "error";
		goto out;
	}

out:
	if (rc) {
		metal_log(METAL_LOG_ERROR, "%s", err_msg);
		rc = -EINVAL;
	}
	return rc;
}

