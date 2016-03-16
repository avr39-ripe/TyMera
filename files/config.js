function get_config() {
	$.getJSON('/config.json',
			function(data) {
				$.each(data, function(key, value){
            		document.getElementById(key).value = value;
            	if (data.StaEnable == 1) {
            		document.getElementById('StaEnable').checked = true;
            	}
            	else
            		document.getElementById('StaEnable').checked = false;
        		});
            });
}


function post_netcfg(event) {
	event.preventDefault();
	var formData = {
			'StaSSID'					:	document.getElementById('StaSSID').value,
			'StaPassword'				:	document.getElementById('StaPassword').value,
			'StaEnable'					:	(document.getElementById('StaEnable').checked ? 1 : 0)
			};
	$.ajax({
        type        : 'POST',
        url         : '/config',
        contentType	: 'application/json; charset=utf-8',
        data        : JSON.stringify(formData),
        dataType	: 'json'
    })
}

// function post_config(event) {
	// event.preventDefault();
	// var formData = {
			// 'sensorUrl'			:	document.getElementById('sensorUrl').value
			// };
	// $.ajax({
        // type        : 'POST',
        // url         : '/config',
        // contentType	: 'application/json; charset=utf-8',
        // data        : JSON.stringify(formData),
        // dataType	: 'json'
    // })
// }

function post_config(event) {
	event.preventDefault();
	var formData = {
			'mode_switch_temp'			:	document.getElementById('mode_switch_temp').value,
			'mode_switch_temp_delta'	:	document.getElementById('mode_switch_temp_delta').value,
			'pump_on_delay'				:	document.getElementById('pump_on_delay').value,
			'pump_off_delay'			:	document.getElementById('pump_off_delay').value,
			'caldron_on_delay'			:	document.getElementById('caldron_on_delay').value,
			'room_off_delay'			:	document.getElementById('room_off_delay').value,
			'twvalve_temp'				:	document.getElementById('twvalve_temp').value,
			'twvalve_temp_delta'		:	document.getElementById('twvalve_temp_delta').value,
			'twvalve_step_time'			:	document.getElementById('twvalve_step_time').value,
			'twvalve_edge_time'			:	document.getElementById('twvalve_edge_time').value
			};
	$.ajax({
        type        : 'POST',
        url         : '/config',
        data        : formData
   });
}

$( document ).ready(function() {
	get_config();
	
	document.getElementById('form_netcfg').addEventListener('submit', post_netcfg);
	document.getElementById('netcfg_cancel').addEventListener('click', get_config);
	document.getElementById('form_settings').addEventListener('submit', post_config);
	document.getElementById('settings_cancel').addEventListener('click', get_config);
});