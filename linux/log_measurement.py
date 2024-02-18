from drone_comm import *
import time

class log_measurement:
    def __init__(self, filename):
        self.filename = filename
        self.start_time = time.time()
        self.measurements = ""

    def log(self,ds):
        self.measurements += f"#########################\n"
        self.measurements += f"Time: {str(time.time()-self.start_time)}\n"
        self.measurements += f"roll: {str(ds.roll)}\n"
        self.measurements += f"pitch: {str(ds.pitch)}\n"
        self.measurements += f"yaw: {str(ds.yaw)}\n"
        self.measurements += f"Height Correction: {str(ds.height_correction)}\n"
        self.measurements += f"Yaw Correction: {str(ds.yaw_correction)}\n"
        self.measurements += f"Roll Correction: {str(ds.roll_correction)}\n"
        self.measurements += f"Pitch Correction: {str(ds.pitch_correction)}\n"
        self.measurements += f"Target height: {str(ds.target_height)}\n"
        self.measurements += f"height: {str(ds.height)}\n"

        if len(self.measurements) >= 4096:
            with open(self.filename,"a") as f:
                f.write(self.measurements)
    
    def close(self):
        with open(self.filename,"a") as f:
            f.write(self.measurements)