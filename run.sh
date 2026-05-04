qemu-system-aarch64 \
  -M raspi4b \
  -kernel bin/kernel8.img \
  -smp 4 \
  -m 2G \
  -nographic
# -d in_asm,exec,cpu
