#!/bin/bash

# 指定新的 GRUB_CMDLINE_LINUX_DEFAULT 值
new_grub_cmdline="quiet splash usbcore.usbfs_memory_mb=1024"

# 获取当前 GRUB_CMDLINE_LINUX_DEFAULT 值
current_grub_cmdline=$(grep "GRUB_CMDLINE_LINUX_DEFAULT" /etc/default/grub | sed 's/GRUB_CMDLINE_LINUX_DEFAULT=//')

# 如果找到 GRUB_CMDLINE_LINUX_DEFAULT 并且值不为空，则进行替换
if [ -n "$current_grub_cmdline" ]; then
    if sudo sed -i "s/GRUB_CMDLINE_LINUX_DEFAULT=.*/GRUB_CMDLINE_LINUX_DEFAULT=\"$new_grub_cmdline\"/" /etc/default/grub; then
        echo "GRUB_CMDLINE_LINUX_DEFAULT updated successfully"
    else
        echo "Failed to update GRUB_CMDLINE_LINUX_DEFAULT"
        exit 1
    fi
else
    # 如果未找到 GRUB_CMDLINE_LINUX_DEFAULT，则将其添加到文件末尾
    if sudo bash -c "echo 'GRUB_CMDLINE_LINUX_DEFAULT=\"$new_grub_cmdline\"' >> /etc/default/grub"; then
        echo "GRUB_CMDLINE_LINUX_DEFAULT added successfully"
    else
        echo "Failed to add GRUB_CMDLINE_LINUX_DEFAULT"
        exit 1
    fi
fi

# 更新 GRUB 配置
if sudo update-grub; then
    echo "GRUB configuration updated successfully"
else
    echo "Failed to update GRUB configuration"
    exit 1
fi

echo "All done!"

