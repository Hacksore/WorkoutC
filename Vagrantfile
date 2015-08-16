VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|

    config.vm.box = "ubuntu/trusty64"

    config.vm.synced_folder ".", "/root/pebble-dev", type: "nfs"
    config.vm.hostname = "pebbledev" 
    		
    config.vm.provider "virtualbox" do |v|
        v.memory = 2048
        v.cpus = 2
	end

    # Shell provisioning
	config.vm.provision "shell" do |s|
        s.path = "init.sh"
    end
end