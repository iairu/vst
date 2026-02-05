import SwiftUI
import CoreAudioKit
import AIVFramework

struct AIVMainView: View {
    @StateObject var viewModel = AudioUnitViewModel()
    var audioUnit: AIVDemo?
    
    var body: some View {
        ZStack {
            Color(red: 0.12, green: 0.12, blue: 0.14).edgesIgnoringSafeArea(.all)
            
            VStack(spacing: 0) {
                // Header
                HStack {
                    Text("AIV VOCAL CHAIN")
                        .font(.system(size: 18, weight: .bold, design: .serif))
                        .foregroundColor(.white)
                        .tracking(2)
                    
                    Spacer()
                    
                    Toggle("BYPASS", isOn: Binding(get: { viewModel.bypass > 0.5 }, set: { viewModel.bypass = $0 ? 1 : 0 }))
                        .toggleStyle(SwitchToggleStyle(tint: .cyan))
                }
                .padding()
                .background(Color(red: 0.15, green: 0.15, blue: 0.17))
                
                // Tab Navigation
                TabView {
                    InputModuleView(viewModel: viewModel)
                        .tabItem {
                            Label("INPUT", systemImage: "slider.vertical.3")
                        }
                    
                    DynamicsModuleView(viewModel: viewModel)
                        .tabItem {
                            Label("DYNAMICS", systemImage: "waveform.path.ecg")
                        }
                    
                    EQModuleView(viewModel: viewModel)
                        .tabItem {
                            Label("EQ", systemImage: "dial.min")
                        }
                    
                    SpatialModuleView(viewModel: viewModel)
                        .tabItem {
                            Label("SPATIAL", systemImage: "dot.radiowaves.left.and.right")
                        }
                }
                .accentColor(.cyan)
                .preferredColorScheme(.dark)
                
                // Global Output
                HStack {
                    Text("OUTPUT").font(.caption).foregroundColor(.gray)
                    ModernSlider(value: $viewModel.gain, range: 0...1, title: "MASTER GAIN")
                }
                .padding(.horizontal)
                .padding(.bottom, 20)
            }
        }
        .onAppear {
            if let au = audioUnit {
                viewModel.connect(audioUnit: au)
            }
        }
    }
}
