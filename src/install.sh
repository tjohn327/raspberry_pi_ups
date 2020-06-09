#!/bin/sh
echo "Checking Python"
if ! command -v python >/dev/null ;
then
    echo "Python not found, please install python"
    exit 1
fi
echo "Python found"

echo "Initializing Power Pi"
python init.py
init_exit_status=$?
if [ "${init_exit_status}" -ne 0 ];
then
    echo "Initialization failed"
    exit 1
fi

_dir="${1:-${PWD}}"
_user="${USER}"
_service="
[Unit]
Description=UPS Service
After=multi-user.target

[Service]
Type=idle
User=${_user}
ExecStart=/usr/bin/python ${_dir}/ups.py
Restart=on-failure

[Install]
WantedBy=multi-user.target
"
_file="/lib/systemd/system/ups.service" 

echo "Creating ups service"
if [ -f "${_file}" ]; 
then
    sudo rm "${_file}"
fi

sudo touch "${_file}"
sudo echo "${_service}" | sudo tee -a "${_file}" > /dev/null

echo "Enabling ups service to run on startup"
sudo systemctl daemon-reload
sudo systemctl enable ups.service
if [ $? != 0 ];
then
    echo "Error enabling ups service"
    exit 1
fi
echo "ups service enabled"
echo "Power Pi configured successfully"
exit 0

