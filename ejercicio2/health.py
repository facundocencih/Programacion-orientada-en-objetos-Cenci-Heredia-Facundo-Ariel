from flask import Flask, jsonify
import subprocess

app = Flask(__name__)

@app.route('/health')
def health():
    try:
        # Uptime
        uptime = subprocess.check_output(['uptime', '-p']).decode('utf-8').strip()
        
        # Load average
        load_output = subprocess.check_output(['uptime']).decode('utf-8')
        load = load_output.split('load average:')[1].split(',')[0].strip() + '%'
        
        # Memory used
        memory_output = subprocess.check_output(['free', '-h']).decode('utf-8')
        memory_line = [line for line in memory_output.split('\n') if 'Mem:' in line][0]
        memory_used = memory_line.split()[2]
        
        # Disk free
        disk_output = subprocess.check_output(['df', '-h', '/']).decode('utf-8')
        disk_line = disk_output.split('\n')[-2]  # Last data line
        disk_free = disk_line.split()[3]
        
        status = 'OK'
        
        return jsonify({
            'uptime': uptime,
            'load': load,
            'memory_used': memory_used,
            'disk_free': disk_free,
            'status': status
        })
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)