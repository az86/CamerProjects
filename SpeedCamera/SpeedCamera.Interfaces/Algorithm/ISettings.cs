namespace SpeedCamera.Interfaces.Algorithm
{
    public interface ISettings
    {
        AZ.Basic.Graphic.Color GreenMax { get; set; }

        AZ.Basic.Graphic.Color GreenMin { get; set; }

        AZ.Basic.Graphic.Color RedMax { get; set; }

        AZ.Basic.Graphic.Color RedMin { get; set; }

        AZ.Basic.Graphic.Color CanvasSize { get; set; }
    }
}
