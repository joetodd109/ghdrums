"""
sampler.py
Joe Todd

This script captures the Guitar Hero Drum Kit MIDI on commands,
and plays the corresponding drum samples.
"""
import os
import pygame
import pygame.midi

KICK = 36    # C3
SNARE = 38   # D3
FLOOR = 45   # A3
HIHAT = 46   # A#3
TOM = 48     # C4
CYMBAL = 49  # C#4


class Sampler:

    def __init__(self, dirname, verbose=False):
        self.verbose = verbose
        self.filedir = os.path.dirname(os.path.realpath(__file__))
        self.load_samples(dirname=dirname)

    def load_samples(self, dirname):
        self.kick = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "Kick.wav"))
        self.snare = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "Snare.wav"))
        self.tom = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "Tom.wav"))
        self.floor = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "Floor.wav"))
        self.hhopen = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "HiHatOpen.wav"))
        #self.hhclosed = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "HiHatClosed.wav"))
        self.ride = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "Ride.wav"))
        self.hicrash = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "HiCrash.wav"))
        self.locrash = pygame.mixer.Sound(os.path.join(self.filedir, dirname, "LoCrash.wav"))

    def play_sample(self, note, velocity):
        print("note = %d, velocity = %d" % (note, velocity))
        if note == KICK:
            self.kick.play()
        elif note == SNARE:
            self.snare.play()
        elif note == TOM:
            self.tom.play()
        elif note == FLOOR:
            self.floor.play()
        elif note == HIHAT:
            if velocity > 50:
                self.hhopen.play()
        elif note == CYMBAL:
            if velocity < 50:
                self.ride.play()
            else:
                self.hicrash.play()
        else:
            pass


if __name__ == '__main__':
    pygame.init()
    pygame.fastevent.init()
    pygame.mixer.init(44100, 16, 1, 4096)
    pygame.midi.init()
    sampler = Sampler(dirname="samples")

    i = pygame.midi.Input(4)

    running = True
    while running:
        try:
            events = pygame.fastevent.get()
            for e in events:
                if e.type in [pygame.midi.MIDIIN]:
                    sampler.play_sample(e.data1, e.data2)

            if i.poll():
                midi_events = i.read(10)
                # convert them into pygame events.
                events = pygame.midi.midis2events(midi_events, i.device_id)

                for evt in events:
                    pygame.fastevent.post(evt)

        except KeyboardInterrupt:
            running = False
            i.close()
            pygame.midi.quit()
            pygame.quit()
